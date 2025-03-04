.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later

.. _pixfmt-reserved:

***************************
Reserved Format Identifiers
***************************

These formats are not defined by this specification, they are just
listed for reference and to avoid naming conflicts. If you want to
register your own format, send an e-mail to the freax-media mailing list
`https://freaxtv.org/lists.php <https://freaxtv.org/lists.php>`__
for inclusion in the ``videodev2.h`` file. If you want to share your
format with other developers add a link to your documentation and send a
copy to the freax-media mailing list for inclusion in this section. If
you think your format should be listed in a standard format section
please make a proposal on the freax-media mailing list.


.. tabularcolumns:: |p{6.6cm}|p{2.2cm}|p{8.5cm}|

.. raw:: latex

    \small

.. _reserved-formats:

.. flat-table:: Reserved Image Formats
    :header-rows:  1
    :stub-columns: 0
    :widths:       3 1 4

    * - Identifier
      - Code
      - Details
    * .. _V4L2-PIX-FMT-DV:

      - ``V4L2_PIX_FMT_DV``
      - 'dvsd'
      - unknown
    * .. _V4L2-PIX-FMT-ET61X251:

      - ``V4L2_PIX_FMT_ET61X251``
      - 'E625'
      - Compressed format of the ET61X251 driver.
    * .. _V4L2-PIX-FMT-HI240:

      - ``V4L2_PIX_FMT_HI240``
      - 'HI24'
      - 8 bit RGB format used by the BTTV driver.
    * .. _V4L2-PIX-FMT-CPIA1:

      - ``V4L2_PIX_FMT_CPIA1``
      - 'CPIA'
      - YUV format used by the gspca cpia1 driver.
    * .. _V4L2-PIX-FMT-JPGL:

      - ``V4L2_PIX_FMT_JPGL``
      - 'JPGL'
      - JPEG-Light format (Pegasus Lossless JPEG) used in Divio webcams NW
	80x.
    * .. _V4L2-PIX-FMT-SPCA501:

      - ``V4L2_PIX_FMT_SPCA501``
      - 'S501'
      - YUYV per line used by the gspca driver.
    * .. _V4L2-PIX-FMT-SPCA505:

      - ``V4L2_PIX_FMT_SPCA505``
      - 'S505'
      - YYUV per line used by the gspca driver.
    * .. _V4L2-PIX-FMT-SPCA508:

      - ``V4L2_PIX_FMT_SPCA508``
      - 'S508'
      - YUVY per line used by the gspca driver.
    * .. _V4L2-PIX-FMT-SPCA561:

      - ``V4L2_PIX_FMT_SPCA561``
      - 'S561'
      - Compressed GBRG Bayer format used by the gspca driver.
    * .. _V4L2-PIX-FMT-PAC207:

      - ``V4L2_PIX_FMT_PAC207``
      - 'P207'
      - Compressed BGGR Bayer format used by the gspca driver.
    * .. _V4L2-PIX-FMT-MR97310A:

      - ``V4L2_PIX_FMT_MR97310A``
      - 'M310'
      - Compressed BGGR Bayer format used by the gspca driver.
    * .. _V4L2-PIX-FMT-JL2005BCD:

      - ``V4L2_PIX_FMT_JL2005BCD``
      - 'JL20'
      - JPEG compressed RGGB Bayer format used by the gspca driver.
    * .. _V4L2-PIX-FMT-OV511:

      - ``V4L2_PIX_FMT_OV511``
      - 'O511'
      - OV511 JPEG format used by the gspca driver.
    * .. _V4L2-PIX-FMT-OV518:

      - ``V4L2_PIX_FMT_OV518``
      - 'O518'
      - OV518 JPEG format used by the gspca driver.
    * .. _V4L2-PIX-FMT-PJPG:

      - ``V4L2_PIX_FMT_PJPG``
      - 'PJPG'
      - Pixart 73xx JPEG format used by the gspca driver.
    * .. _V4L2-PIX-FMT-SE401:

      - ``V4L2_PIX_FMT_SE401``
      - 'S401'
      - Compressed RGB format used by the gspca se401 driver
    * .. _V4L2-PIX-FMT-SQ905C:

      - ``V4L2_PIX_FMT_SQ905C``
      - '905C'
      - Compressed RGGB bayer format used by the gspca driver.
    * .. _V4L2-PIX-FMT-MJPEG:

      - ``V4L2_PIX_FMT_MJPEG``
      - 'MJPG'
      - Compressed format used by the Zoran driver
    * .. _V4L2-PIX-FMT-PWC1:

      - ``V4L2_PIX_FMT_PWC1``
      - 'PWC1'
      - Compressed format of the PWC driver.
    * .. _V4L2-PIX-FMT-PWC2:

      - ``V4L2_PIX_FMT_PWC2``
      - 'PWC2'
      - Compressed format of the PWC driver.
    * .. _V4L2-PIX-FMT-SN9C10X:

      - ``V4L2_PIX_FMT_SN9C10X``
      - 'S910'
      - Compressed format of the SN9C102 driver.
    * .. _V4L2-PIX-FMT-SN9C20X-I420:

      - ``V4L2_PIX_FMT_SN9C20X_I420``
      - 'S920'
      - YUV 4:2:0 format of the gspca sn9c20x driver.
    * .. _V4L2-PIX-FMT-SN9C2028:

      - ``V4L2_PIX_FMT_SN9C2028``
      - 'SONX'
      - Compressed GBRG bayer format of the gspca sn9c2028 driver.
    * .. _V4L2-PIX-FMT-STV0680:

      - ``V4L2_PIX_FMT_STV0680``
      - 'S680'
      - Bayer format of the gspca stv0680 driver.
    * .. _V4L2-PIX-FMT-WNVA:

      - ``V4L2_PIX_FMT_WNVA``
      - 'WNVA'
      - Used by the Winnov Videum driver,
	`http://www.thedirks.org/winnov/ <http://www.thedirks.org/winnov/>`__
    * .. _V4L2-PIX-FMT-TM6000:

      - ``V4L2_PIX_FMT_TM6000``
      - 'TM60'
      - Used by Trident tm6000
    * .. _V4L2-PIX-FMT-CIT-YYVYUY:

      - ``V4L2_PIX_FMT_CIT_YYVYUY``
      - 'CITV'
      - Used by xirlink CIT, found at IBM webcams.

	Uses one line of Y then 1 line of VYUY
    * .. _V4L2-PIX-FMT-KONICA420:

      - ``V4L2_PIX_FMT_KONICA420``
      - 'KONI'
      - Used by Konica webcams.

	YUV420 planar in blocks of 256 pixels.
    * .. _V4L2-PIX-FMT-YYUV:

      - ``V4L2_PIX_FMT_YYUV``
      - 'YYUV'
      - unknown
    * .. _V4L2-PIX-FMT-Y4:

      - ``V4L2_PIX_FMT_Y4``
      - 'Y04 '
      - Old 4-bit greyscale format. Only the most significant 4 bits of
	each byte are used, the other bits are set to 0.
    * .. _V4L2-PIX-FMT-Y6:

      - ``V4L2_PIX_FMT_Y6``
      - 'Y06 '
      - Old 6-bit greyscale format. Only the most significant 6 bits of
	each byte are used, the other bits are set to 0.
    * .. _V4L2-PIX-FMT-S5C-UYVY-JPG:

      - ``V4L2_PIX_FMT_S5C_UYVY_JPG``
      - 'S5CI'
      - Two-planar format used by Samsung S5C73MX cameras. The first plane
	contains interleaved JPEG and UYVY image data, followed by meta
	data in form of an array of offsets to the UYVY data blocks. The
	actual pointer array follows immediately the interleaved JPEG/UYVY
	data, the number of entries in this array equals the height of the
	UYVY image. Each entry is a 4-byte unsigned integer in big endian
	order and it's an offset to a single pixel line of the UYVY image.
	The first plane can start either with JPEG or UYVY data chunk. The
	size of a single UYVY block equals the UYVY image's width
	multiplied by 2. The size of a JPEG chunk depends on the image and
	can vary with each line.

	The second plane, at an offset of 4084 bytes, contains a 4-byte
	offset to the pointer array in the first plane. This offset is
	followed by a 4-byte value indicating size of the pointer array.
	All numbers in the second plane are also in big endian order.
	Remaining data in the second plane is undefined. The information
	in the second plane allows to easily find location of the pointer
	array, which can be different for each frame. The size of the
	pointer array is constant for given UYVY image height.

	In order to extract UYVY and JPEG frames an application can
	initially set a data pointer to the start of first plane and then
	add an offset from the first entry of the pointers table. Such a
	pointer indicates start of an UYVY image pixel line. Whole UYVY
	line can be copied to a separate buffer. These steps should be
	repeated for each line, i.e. the number of entries in the pointer
	array. Anything what's in between the UYVY lines is JPEG data and
	should be concatenated to form the JPEG stream.
    * .. _V4L2-PIX-FMT-MT21C:

      - ``V4L2_PIX_FMT_MT21C``
      - 'MT21'
      - Compressed two-planar YVU420 format used by Mediatek MT8173, MT8192,
        MT8195 and more. The compression is lossless. This format have
	similitude with ``V4L2_PIX_FMT_MM21`` in term of alignment and tiling.
	It remains an opaque intermediate format and the MDP hardware must be
	used to convert ``V4L2_PIX_FMT_MT21C`` to ``V4L2_PIX_FMT_NV12M``,
	``V4L2_PIX_FMT_YUV420M`` or ``V4L2_PIX_FMT_YVU420``.
    * .. _V4L2-PIX-FMT-QC08C:

      - ``V4L2_PIX_FMT_QC08C``
      - 'QC08C'
      - Compressed Macro-tile 8-Bit YUV420 format used by Qualcomm platforms.
        It is an opaque intermediate format. The used compression is lossless
        and it is used by various multimedia hardware blocks like GPU, display
        controllers, ISP and video accelerators.
        It contains four planes for progressive video and eight planes for
        interlaced video.
    * .. _V4L2-PIX-FMT-QC10C:

      - ``V4L2_PIX_FMT_QC10C``
      - 'QC10C'
      - Compressed Macro-tile 10-Bit YUV420 format used by Qualcomm platforms.
        It is an opaque intermediate format. The used compression is lossless
        and it is used by various multimedia hardware blocks like GPU, display
        controllers, ISP and video accelerators.
        It contains four planes for progressive video.
    * .. _V4L2-PIX-FMT-AJPG:

      - ``V4L2_PIX_FMT_AJPG``
      - 'AJPG'
      - ASPEED JPEG format used by the aspeed-video driver on Aspeed platforms,
        which is generally adapted for remote KVM.
        On each frame compression, I will compare the new frame with previous
        one to decide which macroblock's data is changed, and only the changed
        macroblocks will be compressed.

        The implementation is based on AST2600 A3 datasheet, revision 0.9, which
        is not publicly available. Or you can reference Video stream data format
        – ASPEED mode compression of SDK_User_Guide which available on
        `github <https://github.com/AspeedTech-BMC/openbmc/releases/>`__.

        Decoder's implementation can be found here,
        `aspeed_codec <https://github.com/AspeedTech-BMC/aspeed_codec/>`__
    * .. _V4L2-PIX-FMT-MT2110T:

      - ``V4L2_PIX_FMT_MT2110T``
      - 'MT2110T'
      - This format is two-planar 10-Bit tile mode and having similitude with
        ``V4L2_PIX_FMT_MM21`` in term of alignment and tiling. Used for VP9, AV1
        and HEVC.
    * .. _V4L2-PIX-FMT-MT2110R:

      - ``V4L2_PIX_FMT_MT2110R``
      - 'MT2110R'
      - This format is two-planar 10-Bit raster mode and having similitude with
        ``V4L2_PIX_FMT_MM21`` in term of alignment and tiling. Used for AVC.
.. raw:: latex

    \normalsize
