.ifndef __core_config_s
	.define __core_config_s, 1
	.define _TOOL_CONFIG_VER, 10074
	.define	core_config_cir_identity,0x00000044
	.define	core_config_cir_identity_chipid,0
	.define	core_config_cir_identity_arcnum,0
	.define	core_config_cir_identity_arcver,68
	.define	core_config_cir_identity_family,4
	.define	core_config_cir_identity_corever,4
	.define	core_config_cir_aux_dccm,0x80000000
	.define	core_config_bcr_bcr_ver,0x00000002
	.define	core_config_bcr_bcr_ver_version,2
	.define	core_config_bcr_vecbase_ac_build,0x00000010
	.define	core_config_bcr_vecbase_ac_build_version,4
	.define	core_config_bcr_vecbase_ac_build_vector_config,0
	.define	core_config_bcr_vecbase_ac_build_addr,0
	.define	core_config_bcr_rf_build,0x00000102
	.define	core_config_bcr_rf_build_version,2
	.define	core_config_bcr_rf_build_p,1
	.define	core_config_bcr_rf_build_e,0
	.define	core_config_bcr_rf_build_r,0
	.define	core_config_bcr_rf_build_b,0
	.define	core_config_bcr_rf_build_d,0
	.define	core_config_bcr_dccm_build,0x00010b04
	.define	core_config_bcr_dccm_build_w,0
	.define	core_config_bcr_dccm_build_cycles,0
	.define	core_config_bcr_dccm_build_interleave,1
	.define	core_config_bcr_dccm_build_size1,0
	.define	core_config_bcr_dccm_build_size0,11
	.define	core_config_bcr_dccm_build_version,4
	.define	core_config_bcr_timer_build,0x00110704
	.define	core_config_bcr_timer_build_sp1,0
	.define	core_config_bcr_timer_build_sp0,0
	.define	core_config_bcr_timer_build_p1,1
	.define	core_config_bcr_timer_build_p0,1
	.define	core_config_bcr_timer_build_st1,0
	.define	core_config_bcr_timer_build_st0,0
	.define	core_config_bcr_timer_build_rtc,1
	.define	core_config_bcr_timer_build_rtsc_ver,17
	.define	core_config_bcr_timer_build_rtsc,0
	.define	core_config_bcr_timer_build_t0,1
	.define	core_config_bcr_timer_build_t1,1
	.define	core_config_bcr_timer_build_version,4
	.define	core_config_bcr_ap_build,0x00000005
	.define	core_config_bcr_ap_build_version,5
	.define	core_config_bcr_ap_build_type,0
	.define	core_config_bcr_iccm_build,0x0000b804
	.define	core_config_bcr_iccm_build_w0,0
	.define	core_config_bcr_iccm_build_iccm1_size1,0
	.define	core_config_bcr_iccm_build_iccm0_size1,0
	.define	core_config_bcr_iccm_build_iccm1_size0,11
	.define	core_config_bcr_iccm_build_iccm0_size0,8
	.define	core_config_bcr_iccm_build_version,4
	.define	core_config_bcr_xy_build,0x00003220
	.define	core_config_bcr_xy_build_memsize,3
	.define	core_config_bcr_xy_build_interleaved,0
	.define	core_config_bcr_xy_build_config,2
	.define	core_config_bcr_xy_build_version,32
	.define	core_config_bcr_dsp_build,0x00003521
	.define	core_config_bcr_dsp_build_wide,0
	.define	core_config_bcr_dsp_build_itu_pa,1
	.define	core_config_bcr_dsp_build_acc_shift,2
	.define	core_config_bcr_dsp_build_comp,1
	.define	core_config_bcr_dsp_build_divsqrt,1
	.define	core_config_bcr_dsp_build_version,33
	.define	core_config_bcr_multiply_build,0x00022206
	.define	core_config_bcr_multiply_build_version16x16,2
	.define	core_config_bcr_multiply_build_dsp,2
	.define	core_config_bcr_multiply_build_cyc,0
	.define	core_config_bcr_multiply_build_type,2
	.define	core_config_bcr_multiply_build_version32x32,6
	.define	core_config_bcr_swap_build,0x00000003
	.define	core_config_bcr_swap_build_version,3
	.define	core_config_bcr_norm_build,0x00000003
	.define	core_config_bcr_norm_build_version,3
	.define	core_config_bcr_minmax_build,0x00000002
	.define	core_config_bcr_minmax_build_version,2
	.define	core_config_bcr_barrel_build,0x00000303
	.define	core_config_bcr_barrel_build_version,3
	.define	core_config_bcr_barrel_build_shift_option,3
	.define	core_config_bcr_isa_config,0x12447402
	.define	core_config_bcr_isa_config_res1,0
	.define	core_config_bcr_isa_config_d,1
	.define	core_config_bcr_isa_config_res2,0
	.define	core_config_bcr_isa_config_f,0
	.define	core_config_bcr_isa_config_c,2
	.define	core_config_bcr_isa_config_l,0
	.define	core_config_bcr_isa_config_n,1
	.define	core_config_bcr_isa_config_a,0
	.define	core_config_bcr_isa_config_b,0
	.define	core_config_bcr_isa_config_addr_size,4
	.define	core_config_bcr_isa_config_lpc_size,7
	.define	core_config_bcr_isa_config_pc_size,4
	.define	core_config_bcr_isa_config_version,2
	.define	core_config_bcr_dmp_pp_build,0xf0000012
	.define	core_config_bcr_stack_region_build,0x00000002
	.define	core_config_bcr_erp_build,0x40000004
	.define	core_config_bcr_erp_build_l,0
	.define	core_config_bcr_erp_build_wd,2
	.define	core_config_bcr_erp_build_c,0
	.define	core_config_bcr_erp_build_mmu,0
	.define	core_config_bcr_erp_build_rf,0
	.define	core_config_bcr_erp_build_pc,0
	.define	core_config_bcr_erp_build_ic,0
	.define	core_config_bcr_erp_build_dc,0
	.define	core_config_bcr_erp_build_ip,0
	.define	core_config_bcr_erp_build_dp,0
	.define	core_config_bcr_erp_build_version,4
	.define	core_config_bcr_fpu_build,0x00004f03
	.define	core_config_bcr_fpu_build_da,0
	.define	core_config_bcr_fpu_build_hp,0
	.define	core_config_bcr_fpu_build_dd,0
	.define	core_config_bcr_fpu_build_dc,0
	.define	core_config_bcr_fpu_build_df,0
	.define	core_config_bcr_fpu_build_dp,0
	.define	core_config_bcr_fpu_build_fd_v1,2
	.define	core_config_bcr_fpu_build_pi,1
	.define	core_config_bcr_fpu_build_fd,0
	.define	core_config_bcr_fpu_build_fm,0
	.define	core_config_bcr_fpu_build_sd,1
	.define	core_config_bcr_fpu_build_sc,1
	.define	core_config_bcr_fpu_build_sf,1
	.define	core_config_bcr_fpu_build_sp,1
	.define	core_config_bcr_fpu_build_version,3
	.define	core_config_bcr_cprot_build,0x00000001
	.define	core_config_bcr_agu_build,0x01988c02
	.define	core_config_bcr_agu_build_accordian,1
	.define	core_config_bcr_agu_build_wb_size,4
	.define	core_config_bcr_agu_build_num_modifier,24
	.define	core_config_bcr_agu_build_num_offset,8
	.define	core_config_bcr_agu_build_num_addr,12
	.define	core_config_bcr_agu_build_version,2
	.define	core_config_bcr_dmac_build,0x00160f02
	.define	core_config_bcr_dmac_build_int_cfg,2
	.define	core_config_bcr_dmac_build_fifo,3
	.define	core_config_bcr_dmac_build_chan_mem,0
	.define	core_config_bcr_dmac_build_channels,15
	.define	core_config_bcr_dmac_build_version,2
	.define	core_config_bcr_subsys_build,0x00101083
	.define	core_config_bcr_subsys_build_version_major,0
	.define	core_config_bcr_subsys_build_version_minor,2
	.define	core_config_bcr_subsys_build_version_build,8
	.define	core_config_bcr_subsys_build_type,3
	.define	core_config_bcr_core_config,0x00000101
	.define	core_config_bcr_core_config_turbo_boost,1
	.define	core_config_bcr_core_config_version,1
	.define	core_config_bcr_irq_build,0x116b9e01
	.define	core_config_bcr_irq_build_raz,0
	.define	core_config_bcr_irq_build_nmi,0
	.define	core_config_bcr_irq_build_f,1
	.define	core_config_bcr_irq_build_p,1
	.define	core_config_bcr_irq_build_exts,107
	.define	core_config_bcr_irq_build_irqs,158
	.define	core_config_bcr_irq_build_version,1
	.define	core_config_bcr_pdm_dvfs_build,0x00000102
	.define	core_config_bcr_pdm_dvfs_build_dvfs,0
	.define	core_config_bcr_pdm_dvfs_build_pdm,1
	.define	core_config_bcr_pdm_dvfs_build_version,2
	.define	core_config_bcr_ifqueue_build,0x00000302
	.define	core_config_bcr_ifqueue_build_bd,3
	.define	core_config_bcr_ifqueue_build_version,2
	.define	core_config_bcr_smart_build,0x00008003
	.define	core_config_bcr_smart_build_version,3
	.define	core_config_bcr_smart_build_stack_size,32
	.define	core_config_cir_aux_iccm,0x01000000
	.define	core_config_cir_xccm_base,0x90000000
	.define	core_config_cir_yccm_base,0xa0000000
	.define	core_config_cir_subsys_dsp_0_build,0x00000000
	.define	core_config_cir_subsys_io_0_build,0x17100030
	.define	core_config_cir_subsys_io_1_build,0x00000000
	.define	core_config_cir_subsys_io_2_build,0x00000100
	.define	core_config_cir_subsys_uaux_offset,0x00100000
	.define	core_config_cir_subsys_apex_offset,0x80000000
	.define	core_config_family,4
	.define	core_config_core_version,4
	.define	core_config_family_name,"arcv2em"
	.define	core_config_rgf_num_banks,1
	.define	core_config_rgf_num_wr_ports,2
	.define	core_config_endian,"little"
	.define	core_config_endian_little,1
	.define	core_config_endian_big,0
	.define	core_config_lpc_size,32
	.define	core_config_pc_size,32
	.define	core_config_addr_size,32
	.define	core_config_unaligned,1
	.define	core_config_code_density,1
	.define	core_config_div_rem,"radix2"
	.define	core_config_div_rem_radix2,1
	.define	core_config_turbo_boost,1
	.define	core_config_swap,1
	.define	core_config_bitscan,1
	.define	core_config_mpy_option,"mpyd"
	.define	core_config_mpy_option_num,8
	.define	core_config_shift_assist,1
	.define	core_config_barrel_shifter,1
	.define	core_config_dsp,1
	.define	core_config_dsp2,1
	.define	core_config_dsp_complex,1
	.define	core_config_dsp_divsqrt,"radix2"
	.define	core_config_dsp_divsqrt_radix2,1
	.define	core_config_dsp_itu,1
	.define	core_config_dsp_accshift,"full"
	.define	core_config_dsp_accshift_full,1
	.define	core_config_agu_large,1
	.define	core_config_agu_wb_depth,4
	.define	core_config_agu_accord,1
	.define	core_config_xy,1
	.define	core_config_xy_config,"dccm_x_y"
	.define	core_config_xy_config_dccm_x_y,1
	.define	core_config_xy_size,32768
	.define	core_config_xy_size_KM,"32K"
	.define	core_config_xy_x_base,0x90000000
	.define	core_config_xy_y_base,0xa0000000
	.define	core_config_fpus_div,1
	.define	core_config_fpu_mac,1
	.define	core_config_fpus_mpy_slow,1
	.define	core_config_fpus_div_slow,1
	.define	core_config_fpu_pipe_impl,1
	.define	core_config_timer0,1
	.define	core_config_timer0_level,1
	.define	core_config_timer0_vector,16
	.define	core_config_timer1,1
	.define	core_config_timer1_level,1
	.define	core_config_timer1_vector,17
	.define	core_config_rtc,1
	.define	core_config_action_points,2
	.define	core_config_ap_feature,1
	.define	core_config_stack_check,1
	.define	core_config_code_protection,1
	.define	core_config_smart_stack_entries,32
	.define	core_config_ifq_present,1
	.define	core_config_ifq_entries,8
	.define	core_config_interrupts_present,1
	.define	core_config_interrupts_number,158
	.define	core_config_interrupts_priorities,2
	.define	core_config_interrupts_externals,107
	.define	core_config_interrupts,158
	.define	core_config_interrupt_priorities,2
	.define	core_config_ext_interrupts,107
	.define	core_config_interrupts_firq,1
	.define	core_config_interrupts_base,0x0
	.define	core_config_dccm_present,1
	.define	core_config_dccm_size,0x80000
	.define	core_config_dccm_base,0x80000000
	.define	core_config_dccm_interleave,1
	.define	core_config_iccm_present,1
	.define	core_config_iccm0_present,1
	.define	core_config_iccm_size,0x10000
	.define	core_config_iccm0_size,0x10000
	.define	core_config_iccm1_present,1
	.define	core_config_iccm1_size,0x80000
	.define	core_config_iccm_base,0x00000000
	.define	core_config_iccm0_base,0x00000000
	.define	core_config_iccm1_base,0x10000000
	.define	core_config_error_prot_ver,4
	.define	core_config_watchdog,1
	.define	core_config_watchdog_size,32
	.define	core_config_dmac,1
	.define	core_config_dmac_channels,16
	.define	core_config_dmac_registers,0
	.define	core_config_dmac_fifo_depth,4
	.define	core_config_dmac_int_config,"multiple_internal"
	.define	core_config_power_domains,1
	.define	core_config_clock_speed,400
.endif ; __core_config_s

