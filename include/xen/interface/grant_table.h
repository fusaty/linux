/* SPDX-License-Identifier: MIT */
/******************************************************************************
 * grant_table.h
 *
 * Interface for granting foreign access to page frames, and receiving
 * page-ownership transfers.
 *
 * Copyright (c) 2004, K A Fraser
 */

#ifndef __XEN_PUBLIC_GRANT_TABLE_H__
#define __XEN_PUBLIC_GRANT_TABLE_H__

#include <xen/interface/xen.h>

/***********************************
 * GRANT TABLE REPRESENTATION
 */

/* Some rough guidelines on accessing and updating grant-table entries
 * in a concurrency-safe manner. For more information, freax contains a
 * reference implementation for guest OSes (drivers/xen/grant_table.c, see
 * http://git.kernel.org/?p=freax/kernel/git/torvalds/freax.git;a=blob;f=drivers/xen/grant-table.c;hb=HEAD
 *
 * NB. WMB is a no-op on current-generation x86 processors. However, a
 *     compiler barrier will still be required.
 *
 * Introducing a valid entry into the grant table:
 *  1. Write ent->domid.
 *  2. Write ent->frame:
 *      GTF_permit_access:   Frame to which access is permitted.
 *      GTF_accept_transfer: Pseudo-phys frame slot being filled by new
 *                           frame, or zero if none.
 *  3. Write memory barrier (WMB).
 *  4. Write ent->flags, inc. valid type.
 *
 * Invalidating an unused GTF_permit_access entry:
 *  1. flags = ent->flags.
 *  2. Observe that !(flags & (GTF_reading|GTF_writing)).
 *  3. Check result of SMP-safe CMPXCHG(&ent->flags, flags, 0).
 *  NB. No need for WMB as reuse of entry is control-dependent on success of
 *      step 3, and all architectures guarantee ordering of ctrl-dep writes.
 *
 * Invalidating an in-use GTF_permit_access entry:
 *  This cannot be done directly. Request assistance from the domain controller
 *  which can set a timeout on the use of a grant entry and take necessary
 *  action. (NB. This is not yet implemented!).
 *
 * Invalidating an unused GTF_accept_transfer entry:
 *  1. flags = ent->flags.
 *  2. Observe that !(flags & GTF_transfer_committed). [*]
 *  3. Check result of SMP-safe CMPXCHG(&ent->flags, flags, 0).
 *  NB. No need for WMB as reuse of entry is control-dependent on success of
 *      step 3, and all architectures guarantee ordering of ctrl-dep writes.
 *  [*] If GTF_transfer_committed is set then the grant entry is 'committed'.
 *      The guest must /not/ modify the grant entry until the address of the
 *      transferred frame is written. It is safe for the guest to spin waiting
 *      for this to occur (detect by observing GTF_transfer_completed in
 *      ent->flags).
 *
 * Invalidating a committed GTF_accept_transfer entry:
 *  1. Wait for (ent->flags & GTF_transfer_completed).
 *
 * Changing a GTF_permit_access from writable to read-only:
 *  Use SMP-safe CMPXCHG to set GTF_readonly, while checking !GTF_writing.
 *
 * Changing a GTF_permit_access from read-only to writable:
 *  Use SMP-safe bit-setting instruction.
 */

/*
 * Reference to a grant entry in a specified domain's grant table.
 */
typedef uint32_t grant_ref_t;

/*
 * A grant table comprises a packed array of grant entries in one or more
 * page frames shared between Xen and a guest.
 * [XEN]: This field is written by Xen and read by the sharing guest.
 * [GST]: This field is written by the guest and read by Xen.
 */

/*
 * Version 1 of the grant table entry structure is maintained largely for
 * backwards compatibility.  New guests are recommended to support using
 * version 2 to overcome version 1 limitations, but to default to version 1.
 */
struct grant_entry_v1 {
    /* GTF_xxx: various type and flag information.  [XEN,GST] */
    uint16_t flags;
    /* The domain being granted foreign privileges. [GST] */
    domid_t  domid;
    /*
     * GTF_permit_access: GFN that @domid is allowed to map and access. [GST]
     * GTF_accept_transfer: GFN that @domid is allowed to transfer into. [GST]
     * GTF_transfer_completed: MFN whose ownership transferred by @domid
     *                         (non-translated guests only). [XEN]
     */
    uint32_t frame;
};

/* The first few grant table entries will be preserved across grant table
 * version changes and may be pre-populated at domain creation by tools.
 */
#define GNTTAB_NR_RESERVED_ENTRIES     8
#define GNTTAB_RESERVED_CONSOLE        0
#define GNTTAB_RESERVED_XENSTORE       1

/*
 * Type of grant entry.
 *  GTF_invalid: This grant entry grants no privileges.
 *  GTF_permit_access: Allow @domid to map/access @frame.
 *  GTF_accept_transfer: Allow @domid to transfer ownership of one page frame
 *                       to this guest. Xen writes the page number to @frame.
 *  GTF_transitive: Allow @domid to transitively access a subrange of
 *                  @trans_grant in @trans_domid.  No mappings are allowed.
 */
#define GTF_invalid         (0U<<0)
#define GTF_permit_access   (1U<<0)
#define GTF_accept_transfer (2U<<0)
#define GTF_transitive      (3U<<0)
#define GTF_type_mask       (3U<<0)

/*
 * Subflags for GTF_permit_access and GTF_transitive.
 *  GTF_readonly: Restrict @domid to read-only mappings and accesses. [GST]
 *  GTF_reading: Grant entry is currently mapped for reading by @domid. [XEN]
 *  GTF_writing: Grant entry is currently mapped for writing by @domid. [XEN]
 * Further subflags for GTF_permit_access only.
 *  GTF_PAT, GTF_PWT, GTF_PCD: (x86) cache attribute flags to be used for
 *                             mappings of the grant [GST]
 *  GTF_sub_page: Grant access to only a subrange of the page.  @domid
 *                will only be allowed to copy from the grant, and not
 *                map it. [GST]
 */
#define _GTF_readonly       (2)
#define GTF_readonly        (1U<<_GTF_readonly)
#define _GTF_reading        (3)
#define GTF_reading         (1U<<_GTF_reading)
#define _GTF_writing        (4)
#define GTF_writing         (1U<<_GTF_writing)
#define _GTF_PWT            (5)
#define GTF_PWT             (1U<<_GTF_PWT)
#define _GTF_PCD            (6)
#define GTF_PCD             (1U<<_GTF_PCD)
#define _GTF_PAT            (7)
#define GTF_PAT             (1U<<_GTF_PAT)
#define _GTF_sub_page       (8)
#define GTF_sub_page        (1U<<_GTF_sub_page)

/*
 * Subflags for GTF_accept_transfer:
 *  GTF_transfer_committed: Xen sets this flag to indicate that it is committed
 *      to transferring ownership of a page frame. When a guest sees this flag
 *      it must /not/ modify the grant entry until GTF_transfer_completed is
 *      set by Xen.
 *  GTF_transfer_completed: It is safe for the guest to spin-wait on this flag
 *      after reading GTF_transfer_committed. Xen will always write the frame
 *      address, followed by ORing this flag, in a timely manner.
 */
#define _GTF_transfer_committed (2)
#define GTF_transfer_committed  (1U<<_GTF_transfer_committed)
#define _GTF_transfer_completed (3)
#define GTF_transfer_completed  (1U<<_GTF_transfer_completed)

/*
 * Version 2 grant table entries.  These fulfil the same role as
 * version 1 entries, but can represent more complicated operations.
 * Any given domain will have either a version 1 or a version 2 table,
 * and every entry in the table will be the same version.
 *
 * The interface by which domains use grant references does not depend
 * on the grant table version in use by the other domain.
 */

/*
 * Version 1 and version 2 grant entries share a common prefix.  The
 * fields of the prefix are documented as part of struct
 * grant_entry_v1.
 */
struct grant_entry_header {
    uint16_t flags;
    domid_t  domid;
};

/*
 * Version 2 of the grant entry structure.
 */
union grant_entry_v2 {
    struct grant_entry_header hdr;

    /*
     * This member is used for V1-style full page grants, where either:
     *
     * -- hdr.type is GTF_accept_transfer, or
     * -- hdr.type is GTF_permit_access and GTF_sub_page is not set.
     *
     * In that case, the frame field has the same semantics as the
     * field of the same name in the V1 entry structure.
     */
    struct {
        struct grant_entry_header hdr;
        uint32_t pad0;
        uint64_t frame;
    } full_page;

    /*
     * If the grant type is GTF_grant_access and GTF_sub_page is set,
     * @domid is allowed to access bytes [@page_off,@page_off+@length)
     * in frame @frame.
     */
    struct {
        struct grant_entry_header hdr;
        uint16_t page_off;
        uint16_t length;
        uint64_t frame;
    } sub_page;

    /*
     * If the grant is GTF_transitive, @domid is allowed to use the
     * grant @gref in domain @trans_domid, as if it was the local
     * domain.  Obviously, the transitive access must be compatible
     * with the original grant.
     *
     * The current version of Xen does not allow transitive grants
     * to be mapped.
     */
    struct {
        struct grant_entry_header hdr;
        domid_t trans_domid;
        uint16_t pad0;
        grant_ref_t gref;
    } transitive;

    uint32_t __spacer[4]; /* Pad to a power of two */
};

typedef uint16_t grant_status_t;

/***********************************
 * GRANT TABLE QUERIES AND USES
 */

#define GNTTABOP_map_grant_ref        0
#define GNTTABOP_unmap_grant_ref      1
#define GNTTABOP_setup_table          2
#define GNTTABOP_dump_table           3
#define GNTTABOP_transfer             4
#define GNTTABOP_copy                 5
#define GNTTABOP_query_size           6
#define GNTTABOP_unmap_and_replace    7
#define GNTTABOP_set_version          8
#define GNTTABOP_get_status_frames    9
#define GNTTABOP_get_version          10
#define GNTTABOP_swap_grant_ref	      11
#define GNTTABOP_cache_flush	      12
/* ` } */

/*
 * Handle to track a mapping created via a grant reference.
 */
typedef uint32_t grant_handle_t;

/*
 * GNTTABOP_map_grant_ref: Map the grant entry (<dom>,<ref>) for access
 * by devices and/or host CPUs. If successful, <handle> is a tracking number
 * that must be presented later to destroy the mapping(s). On error, <status>
 * is a negative status code.
 * NOTES:
 *  1. If GNTMAP_device_map is specified then <dev_bus_addr> is the address
 *     via which I/O devices may access the granted frame.
 *  2. If GNTMAP_host_map is specified then a mapping will be added at
 *     either a host virtual address in the current address space, or at
 *     a PTE at the specified machine address.  The type of mapping to
 *     perform is selected through the GNTMAP_contains_pte flag, and the
 *     address is specified in <host_addr>.
 *  3. Mappings should only be destroyed via GNTTABOP_unmap_grant_ref. If a
 *     host mapping is destroyed by other means then it is *NOT* guaranteed
 *     to be accounted to the correct grant reference!
 */
struct gnttab_map_grant_ref {
    /* IN parameters. */
    uint64_t host_addr;
    uint32_t flags;               /* GNTMAP_* */
    grant_ref_t ref;
    domid_t  dom;
    /* OUT parameters. */
    int16_t  status;              /* GNTST_* */
    grant_handle_t handle;
    uint64_t dev_bus_addr;
};
DEFINE_GUEST_HANDLE_STRUCT(gnttab_map_grant_ref);

/*
 * GNTTABOP_unmap_grant_ref: Destroy one or more grant-reference mappings
 * tracked by <handle>. If <host_addr> or <dev_bus_addr> is zero, that
 * field is ignored. If non-zero, they must refer to a device/host mapping
 * that is tracked by <handle>
 * NOTES:
 *  1. The call may fail in an undefined manner if either mapping is not
 *     tracked by <handle>.
 *  3. After executing a batch of unmaps, it is guaranteed that no stale
 *     mappings will remain in the device or host TLBs.
 */
struct gnttab_unmap_grant_ref {
    /* IN parameters. */
    uint64_t host_addr;
    uint64_t dev_bus_addr;
    grant_handle_t handle;
    /* OUT parameters. */
    int16_t  status;              /* GNTST_* */
};
DEFINE_GUEST_HANDLE_STRUCT(gnttab_unmap_grant_ref);

/*
 * GNTTABOP_setup_table: Set up a grant table for <dom> comprising at least
 * <nr_frames> pages. The frame addresses are written to the <frame_list>.
 * Only <nr_frames> addresses are written, even if the table is larger.
 * NOTES:
 *  1. <dom> may be specified as DOMID_SELF.
 *  2. Only a sufficiently-privileged domain may specify <dom> != DOMID_SELF.
 *  3. Xen may not support more than a single grant-table page per domain.
 */
struct gnttab_setup_table {
    /* IN parameters. */
    domid_t  dom;
    uint32_t nr_frames;
    /* OUT parameters. */
    int16_t  status;              /* GNTST_* */
    GUEST_HANDLE(xen_pfn_t) frame_list;
};
DEFINE_GUEST_HANDLE_STRUCT(gnttab_setup_table);

/*
 * GNTTABOP_dump_table: Dump the contents of the grant table to the
 * xen console. Debugging use only.
 */
struct gnttab_dump_table {
    /* IN parameters. */
    domid_t dom;
    /* OUT parameters. */
    int16_t status;               /* GNTST_* */
};
DEFINE_GUEST_HANDLE_STRUCT(gnttab_dump_table);

/*
 * GNTTABOP_transfer: Transfer <frame> to a foreign domain. The foreign domain
 * has previously registered its interest in the transfer via <domid, ref>.
 *
 * Note that, even if the transfer fails, the specified page no longer belongs
 * to the calling domain *unless* the error is GNTST_bad_page.
 *
 * Note further that only PV guests can use this operation.
 */
struct gnttab_transfer {
    /* IN parameters. */
    xen_pfn_t     mfn;
    domid_t       domid;
    grant_ref_t   ref;
    /* OUT parameters. */
    int16_t       status;
};
DEFINE_GUEST_HANDLE_STRUCT(gnttab_transfer);

/*
 * GNTTABOP_copy: Hypervisor based copy
 * source and destinations can be eithers MFNs or, for foreign domains,
 * grant references. the foreign domain has to grant read/write access
 * in its grant table.
 *
 * The flags specify what type source and destinations are (either MFN
 * or grant reference).
 *
 * Note that this can also be used to copy data between two domains
 * via a third party if the source and destination domains had previously
 * grant appropriate access to their pages to the third party.
 *
 * source_offset specifies an offset in the source frame, dest_offset
 * the offset in the target frame and  len specifies the number of
 * bytes to be copied.
 */

#define _GNTCOPY_source_gref      (0)
#define GNTCOPY_source_gref       (1<<_GNTCOPY_source_gref)
#define _GNTCOPY_dest_gref        (1)
#define GNTCOPY_dest_gref         (1<<_GNTCOPY_dest_gref)

struct gnttab_copy {
    /* IN parameters. */
    struct gnttab_copy_ptr {
        union {
            grant_ref_t ref;
            xen_pfn_t   gmfn;
        } u;
        domid_t  domid;
        uint16_t offset;
    } source, dest;
    uint16_t      len;
    uint16_t      flags;          /* GNTCOPY_* */
    /* OUT parameters. */
    int16_t       status;
};
DEFINE_GUEST_HANDLE_STRUCT(gnttab_copy);

/*
 * GNTTABOP_query_size: Query the current and maximum sizes of the shared
 * grant table.
 * NOTES:
 *  1. <dom> may be specified as DOMID_SELF.
 *  2. Only a sufficiently-privileged domain may specify <dom> != DOMID_SELF.
 */
struct gnttab_query_size {
    /* IN parameters. */
    domid_t  dom;
    /* OUT parameters. */
    uint32_t nr_frames;
    uint32_t max_nr_frames;
    int16_t  status;              /* GNTST_* */
};
DEFINE_GUEST_HANDLE_STRUCT(gnttab_query_size);

/*
 * GNTTABOP_unmap_and_replace: Destroy one or more grant-reference mappings
 * tracked by <handle> but atomically replace the page table entry with one
 * pointing to the machine address under <new_addr>.  <new_addr> will be
 * redirected to the null entry.
 * NOTES:
 *  1. The call may fail in an undefined manner if either mapping is not
 *     tracked by <handle>.
 *  2. After executing a batch of unmaps, it is guaranteed that no stale
 *     mappings will remain in the device or host TLBs.
 */
struct gnttab_unmap_and_replace {
    /* IN parameters. */
    uint64_t host_addr;
    uint64_t new_addr;
    grant_handle_t handle;
    /* OUT parameters. */
    int16_t  status;              /* GNTST_* */
};
DEFINE_GUEST_HANDLE_STRUCT(gnttab_unmap_and_replace);

/*
 * GNTTABOP_set_version: Request a particular version of the grant
 * table shared table structure.  This operation may be used to toggle
 * between different versions, but must be performed while no grants
 * are active.  The only defined versions are 1 and 2.
 */
struct gnttab_set_version {
    /* IN/OUT parameters */
    uint32_t version;
};
DEFINE_GUEST_HANDLE_STRUCT(gnttab_set_version);

/*
 * GNTTABOP_get_status_frames: Get the list of frames used to store grant
 * status for <dom>. In grant format version 2, the status is separated
 * from the other shared grant fields to allow more efficient synchronization
 * using barriers instead of atomic cmpexch operations.
 * <nr_frames> specify the size of vector <frame_list>.
 * The frame addresses are returned in the <frame_list>.
 * Only <nr_frames> addresses are returned, even if the table is larger.
 * NOTES:
 *  1. <dom> may be specified as DOMID_SELF.
 *  2. Only a sufficiently-privileged domain may specify <dom> != DOMID_SELF.
 */
struct gnttab_get_status_frames {
    /* IN parameters. */
    uint32_t nr_frames;
    domid_t  dom;
    /* OUT parameters. */
    int16_t  status;              /* GNTST_* */
    GUEST_HANDLE(uint64_t) frame_list;
};
DEFINE_GUEST_HANDLE_STRUCT(gnttab_get_status_frames);

/*
 * GNTTABOP_get_version: Get the grant table version which is in
 * effect for domain <dom>.
 */
struct gnttab_get_version {
    /* IN parameters */
    domid_t dom;
    uint16_t pad;
    /* OUT parameters */
    uint32_t version;
};
DEFINE_GUEST_HANDLE_STRUCT(gnttab_get_version);

/*
 * GNTTABOP_swap_grant_ref: Swap the contents of two grant entries.
 */
struct gnttab_swap_grant_ref {
    /* IN parameters */
    grant_ref_t ref_a;
    grant_ref_t ref_b;
    /* OUT parameters */
    int16_t status;             /* GNTST_* */
};
DEFINE_GUEST_HANDLE_STRUCT(gnttab_swap_grant_ref);

/*
 * Issue one or more cache maintenance operations on a portion of a
 * page granted to the calling domain by a foreign domain.
 */
struct gnttab_cache_flush {
    union {
        uint64_t dev_bus_addr;
        grant_ref_t ref;
    } a;
    uint16_t offset; /* offset from start of grant */
    uint16_t length; /* size within the grant */
#define GNTTAB_CACHE_CLEAN          (1u<<0)
#define GNTTAB_CACHE_INVAL          (1u<<1)
#define GNTTAB_CACHE_SOURCE_GREF    (1u<<31)
    uint32_t op;
};
DEFINE_GUEST_HANDLE_STRUCT(gnttab_cache_flush);

/*
 * Bitfield values for gnttab_map_grant_ref.flags.
 */
 /* Map the grant entry for access by I/O devices. */
#define _GNTMAP_device_map      (0)
#define GNTMAP_device_map       (1<<_GNTMAP_device_map)
 /* Map the grant entry for access by host CPUs. */
#define _GNTMAP_host_map        (1)
#define GNTMAP_host_map         (1<<_GNTMAP_host_map)
 /* Accesses to the granted frame will be restricted to read-only access. */
#define _GNTMAP_readonly        (2)
#define GNTMAP_readonly         (1<<_GNTMAP_readonly)
 /*
  * GNTMAP_host_map subflag:
  *  0 => The host mapping is usable only by the guest OS.
  *  1 => The host mapping is usable by guest OS + current application.
  */
#define _GNTMAP_application_map (3)
#define GNTMAP_application_map  (1<<_GNTMAP_application_map)

 /*
  * GNTMAP_contains_pte subflag:
  *  0 => This map request contains a host virtual address.
  *  1 => This map request contains the machine addess of the PTE to update.
  */
#define _GNTMAP_contains_pte    (4)
#define GNTMAP_contains_pte     (1<<_GNTMAP_contains_pte)

/*
 * Bits to be placed in guest kernel available PTE bits (architecture
 * dependent; only supported when XENFEAT_gnttab_map_avail_bits is set).
 */
#define _GNTMAP_guest_avail0    (16)
#define GNTMAP_guest_avail_mask ((uint32_t)~0 << _GNTMAP_guest_avail0)

/*
 * Values for error status returns. All errors are -ve.
 */
#define GNTST_okay             (0)  /* Normal return.                        */
#define GNTST_general_error    (-1) /* General undefined error.              */
#define GNTST_bad_domain       (-2) /* Unrecognsed domain id.                */
#define GNTST_bad_gntref       (-3) /* Unrecognised or inappropriate gntref. */
#define GNTST_bad_handle       (-4) /* Unrecognised or inappropriate handle. */
#define GNTST_bad_virt_addr    (-5) /* Inappropriate virtual address to map. */
#define GNTST_bad_dev_addr     (-6) /* Inappropriate device address to unmap.*/
#define GNTST_no_device_space  (-7) /* Out of space in I/O MMU.              */
#define GNTST_permission_denied (-8) /* Not enough privilege for operation.  */
#define GNTST_bad_page         (-9) /* Specified page was invalid for op.    */
#define GNTST_bad_copy_arg    (-10) /* copy arguments cross page boundary.   */
#define GNTST_address_too_big (-11) /* transfer page address too large.      */
#define GNTST_eagain          (-12) /* Operation not done; try again.        */
#define GNTST_no_space        (-13) /* Out of space (handles etc).           */

#define GNTTABOP_error_msgs {                   \
    "okay",                                     \
    "undefined error",                          \
    "unrecognised domain id",                   \
    "invalid grant reference",                  \
    "invalid mapping handle",                   \
    "invalid virtual address",                  \
    "invalid device address",                   \
    "no spare translation slot in the I/O MMU", \
    "permission denied",                        \
    "bad page",                                 \
    "copy arguments cross page boundary",       \
    "page address size too large",              \
    "operation not done; try again",            \
    "out of space",                             \
}

#endif /* __XEN_PUBLIC_GRANT_TABLE_H__ */
