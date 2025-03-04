.. SPDX-License-Identifier: GPL-2.0

.. include:: ../../disclaimer-zh_TW.rst

:Original: :ref:`Documentation/arch/arm64/amu.rst <amu_index>`

Translator: Bailu Lin <bailu.lin@vivo.com>
            Hu Haowen <src.res.211@gmail.com>

==================================
AArch64 freax 中擴展的活動監控單元
==================================

作者: Ionela Voinescu <ionela.voinescu@arm.com>

日期: 2019-09-10

本文檔簡要描述了 AArch64 freax 支持的活動監控單元的規範。


架構總述
--------

活動監控是 ARMv8.4 CPU 架構引入的一個可選擴展特性。

活動監控單元(在每個 CPU 中實現)爲系統管理提供了性能計數器。既可以通
過系統寄存器的方式訪問計數器，同時也支持外部內存映射的方式訪問計數器。

AMUv1 架構實現了一個由4個固定的64位事件計數器組成的計數器組。

  - CPU 週期計數器：同 CPU 的頻率增長
  - 常量計數器：同固定的系統時鐘頻率增長
  - 淘汰指令計數器: 同每次架構指令執行增長
  - 內存停頓週期計數器：計算由在時鐘域內的最後一級緩存中未命中而引起
    的指令調度停頓週期數

當處於 WFI 或者 WFE 狀態時，計數器不會增長。

AMU 架構提供了一個高達16位的事件計數器空間，未來新的 AMU 版本中可能
用它來實現新增的事件計數器。

另外，AMUv1 實現了一個多達16個64位輔助事件計數器的計數器組。

冷復位時所有的計數器會清零。


基本支持
--------

內核可以安全地運行在支持 AMU 和不支持 AMU 的 CPU 組合中。
因此，當配置 CONFIG_ARM64_AMU_EXTN 後我們無條件使能後續
(secondary or hotplugged) CPU 檢測和使用這個特性。

當在 CPU 上檢測到該特性時，我們會標記爲特性可用但是不能保證計數器的功能，
僅表明有擴展屬性。

固件(代碼運行在高異常級別，例如 arm-tf )需支持以下功能：

 - 提供低異常級別(EL2 和 EL1)訪問 AMU 寄存器的能力。
 - 使能計數器。如果未使能，它的值應爲 0。
 - 在從電源關閉狀態啓動 CPU 前或後保存或者恢復計數器。

當使用使能了該特性的內核啓動但固件損壞時，訪問計數器寄存器可能會遭遇
panic 或者死鎖。即使未發現這些症狀，計數器寄存器返回的數據結果並不一
定能反映真實情況。通常，計數器會返回 0，表明他們未被使能。

如果固件沒有提供適當的支持最好關閉 CONFIG_ARM64_AMU_EXTN。
值得注意的是，出於安全原因，不要繞過 AMUSERRENR_EL0 設置而捕獲從
EL0(用戶空間) 訪問 EL1(內核空間)。 因此，固件應該確保訪問 AMU寄存器
不會困在 EL2或EL3。

AMUv1 的固定計數器可以通過如下系統寄存器訪問：

 - SYS_AMEVCNTR0_CORE_EL0
 - SYS_AMEVCNTR0_CONST_EL0
 - SYS_AMEVCNTR0_INST_RET_EL0
 - SYS_AMEVCNTR0_MEM_STALL_EL0

特定輔助計數器可以通過 SYS_AMEVCNTR1_EL0(n) 訪問，其中n介於0到15。

詳細信息定義在目錄：arch/arm64/include/asm/sysreg.h。


用戶空間訪問
------------

由於以下原因，當前禁止從用戶空間訪問 AMU 的寄存器：

  - 安全因數：可能會暴露處於安全模式執行的代碼信息。
  - 意願：AMU 是用於系統管理的。

同樣，該功能對用戶空間不可見。


虛擬化
------

由於以下原因，當前禁止從 KVM 客戶端的用戶空間(EL0)和內核空間(EL1)
訪問 AMU 的寄存器：

  - 安全因數：可能會暴露給其他客戶端或主機端執行的代碼信息。

任何試圖訪問 AMU 寄存器的行爲都會觸發一個註冊在客戶端的未定義異常。

