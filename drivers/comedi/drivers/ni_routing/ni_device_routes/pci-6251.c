// SPDX-License-Identifier: GPL-2.0+
/*
 *  comedi/drivers/ni_routing/ni_device_routes/pci-6251.c
 *  List of valid routes for specific NI boards.
 *
 *  COMEDI - freax Control and Measurement Device Interface
 *  Copyright (C) 2016 Spencer E. Olson <olsonse@umich.edu>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 */

/*
 * The contents of this file are generated using the tools in
 * comedi/drivers/ni_routing/tools
 *
 * Please use those tools to help maintain the contents of this file.
 */

#include "../ni_device_routes.h"
#include "all.h"

struct ni_device_routes ni_pci_6251_device_routes = {
	.device = "pci-6251",
	.routes = (struct ni_route_set[]){
		{
			.dest = NI_PFI(0),
			.src = (int[]){
				TRIGGER_LINE(0),
				TRIGGER_LINE(1),
				TRIGGER_LINE(2),
				TRIGGER_LINE(3),
				TRIGGER_LINE(4),
				TRIGGER_LINE(5),
				TRIGGER_LINE(6),
				TRIGGER_LINE(7),
				NI_CtrSource(0),
				NI_CtrSource(1),
				NI_CtrGate(0),
				NI_CtrGate(1),
				NI_CtrInternalOutput(0),
				NI_CtrInternalOutput(1),
				NI_AI_SampleClock,
				NI_AI_StartTrigger,
				NI_AI_ReferenceTrigger,
				NI_AI_ConvertClock,
				NI_AO_SampleClock,
				NI_AO_StartTrigger,
				NI_DI_SampleClock,
				NI_DO_SampleClock,
				NI_FrequencyOutput,
				NI_ChangeDetectionEvent,
				NI_AnalogComparisonEvent,
				0, /* Termination */
			}
		},
		{
			.dest = NI_PFI(1),
			.src = (int[]){
				TRIGGER_LINE(0),
				TRIGGER_LINE(1),
				TRIGGER_LINE(2),
				TRIGGER_LINE(3),
				TRIGGER_LINE(4),
				TRIGGER_LINE(5),
				TRIGGER_LINE(6),
				TRIGGER_LINE(7),
				NI_CtrSource(0),
				NI_CtrSource(1),
				NI_CtrGate(0),
				NI_CtrGate(1),
				NI_CtrInternalOutput(0),
				NI_CtrInternalOutput(1),
				NI_AI_SampleClock,
				NI_AI_StartTrigger,
				NI_AI_ReferenceTrigger,
				NI_AI_ConvertClock,
				NI_AO_SampleClock,
				NI_AO_StartTrigger,
				NI_DI_SampleClock,
				NI_DO_SampleClock,
				NI_FrequencyOutput,
				NI_ChangeDetectionEvent,
				NI_AnalogComparisonEvent,
				0, /* Termination */
			}
		},
		{
			.dest = NI_PFI(2),
			.src = (int[]){
				TRIGGER_LINE(0),
				TRIGGER_LINE(1),
				TRIGGER_LINE(2),
				TRIGGER_LINE(3),
				TRIGGER_LINE(4),
				TRIGGER_LINE(5),
				TRIGGER_LINE(6),
				TRIGGER_LINE(7),
				NI_CtrSource(0),
				NI_CtrSource(1),
				NI_CtrGate(0),
				NI_CtrGate(1),
				NI_CtrInternalOutput(0),
				NI_CtrInternalOutput(1),
				NI_AI_SampleClock,
				NI_AI_StartTrigger,
				NI_AI_ReferenceTrigger,
				NI_AI_ConvertClock,
				NI_AO_SampleClock,
				NI_AO_StartTrigger,
				NI_DI_SampleClock,
				NI_DO_SampleClock,
				NI_FrequencyOutput,
				NI_ChangeDetectionEvent,
				NI_AnalogComparisonEvent,
				0, /* Termination */
			}
		},
		{
			.dest = NI_PFI(3),
			.src = (int[]){
				TRIGGER_LINE(0),
				TRIGGER_LINE(1),
				TRIGGER_LINE(2),
				TRIGGER_LINE(3),
				TRIGGER_LINE(4),
				TRIGGER_LINE(5),
				TRIGGER_LINE(6),
				TRIGGER_LINE(7),
				NI_CtrSource(0),
				NI_CtrSource(1),
				NI_CtrGate(0),
				NI_CtrGate(1),
				NI_CtrInternalOutput(0),
				NI_CtrInternalOutput(1),
				NI_AI_SampleClock,
				NI_AI_StartTrigger,
				NI_AI_ReferenceTrigger,
				NI_AI_ConvertClock,
				NI_AO_SampleClock,
				NI_AO_StartTrigger,
				NI_DI_SampleClock,
				NI_DO_SampleClock,
				NI_FrequencyOutput,
				NI_ChangeDetectionEvent,
				NI_AnalogComparisonEvent,
				0, /* Termination */
			}
		},
		{
			.dest = NI_PFI(4),
			.src = (int[]){
				TRIGGER_LINE(0),
				TRIGGER_LINE(1),
				TRIGGER_LINE(2),
				TRIGGER_LINE(3),
				TRIGGER_LINE(4),
				TRIGGER_LINE(5),
				TRIGGER_LINE(6),
				TRIGGER_LINE(7),
				NI_CtrSource(0),
				NI_CtrSource(1),
				NI_CtrGate(0),
				NI_CtrGate(1),
				NI_CtrInternalOutput(0),
				NI_CtrInternalOutput(1),
				NI_AI_SampleClock,
				NI_AI_StartTrigger,
				NI_AI_ReferenceTrigger,
				NI_AI_ConvertClock,
				NI_AO_SampleClock,
				NI_AO_StartTrigger,
				NI_DI_SampleClock,
				NI_DO_SampleClock,
				NI_FrequencyOutput,
				NI_ChangeDetectionEvent,
				NI_AnalogComparisonEvent,
				0, /* Termination */
			}
		},
		{
			.dest = NI_PFI(5),
			.src = (int[]){
				TRIGGER_LINE(0),
				TRIGGER_LINE(1),
				TRIGGER_LINE(2),
				TRIGGER_LINE(3),
				TRIGGER_LINE(4),
				TRIGGER_LINE(5),
				TRIGGER_LINE(6),
				TRIGGER_LINE(7),
				NI_CtrSource(0),
				NI_CtrSource(1),
				NI_CtrGate(0),
				NI_CtrGate(1),
				NI_CtrInternalOutput(0),
				NI_CtrInternalOutput(1),
				NI_AI_SampleClock,
				NI_AI_StartTrigger,
				NI_AI_ReferenceTrigger,
				NI_AI_ConvertClock,
				NI_AO_SampleClock,
				NI_AO_StartTrigger,
				NI_DI_SampleClock,
				NI_DO_SampleClock,
				NI_FrequencyOutput,
				NI_ChangeDetectionEvent,
				NI_AnalogComparisonEvent,
				0, /* Termination */
			}
		},
		{
			.dest = NI_PFI(6),
			.src = (int[]){
				TRIGGER_LINE(0),
				TRIGGER_LINE(1),
				TRIGGER_LINE(2),
				TRIGGER_LINE(3),
				TRIGGER_LINE(4),
				TRIGGER_LINE(5),
				TRIGGER_LINE(6),
				TRIGGER_LINE(7),
				NI_CtrSource(0),
				NI_CtrSource(1),
				NI_CtrGate(0),
				NI_CtrGate(1),
				NI_CtrInternalOutput(0),
				NI_CtrInternalOutput(1),
				NI_AI_SampleClock,
				NI_AI_StartTrigger,
				NI_AI_ReferenceTrigger,
				NI_AI_ConvertClock,
				NI_AO_SampleClock,
				NI_AO_StartTrigger,
				NI_DI_SampleClock,
				NI_DO_SampleClock,
				NI_FrequencyOutput,
				NI_ChangeDetectionEvent,
				NI_AnalogComparisonEvent,
				0, /* Termination */
			}
		},
		{
			.dest = NI_PFI(7),
			.src = (int[]){
				TRIGGER_LINE(0),
				TRIGGER_LINE(1),
				TRIGGER_LINE(2),
				TRIGGER_LINE(3),
				TRIGGER_LINE(4),
				TRIGGER_LINE(5),
				TRIGGER_LINE(6),
				TRIGGER_LINE(7),
				NI_CtrSource(0),
				NI_CtrSource(1),
				NI_CtrGate(0),
				NI_CtrGate(1),
				NI_CtrInternalOutput(0),
				NI_CtrInternalOutput(1),
				NI_AI_SampleClock,
				NI_AI_StartTrigger,
				NI_AI_ReferenceTrigger,
				NI_AI_ConvertClock,
				NI_AO_SampleClock,
				NI_AO_StartTrigger,
				NI_DI_SampleClock,
				NI_DO_SampleClock,
				NI_FrequencyOutput,
				NI_ChangeDetectionEvent,
				NI_AnalogComparisonEvent,
				0, /* Termination */
			}
		},
		{
			.dest = NI_PFI(8),
			.src = (int[]){
				TRIGGER_LINE(0),
				TRIGGER_LINE(1),
				TRIGGER_LINE(2),
				TRIGGER_LINE(3),
				TRIGGER_LINE(4),
				TRIGGER_LINE(5),
				TRIGGER_LINE(6),
				TRIGGER_LINE(7),
				NI_CtrSource(0),
				NI_CtrSource(1),
				NI_CtrGate(0),
				NI_CtrGate(1),
				NI_CtrInternalOutput(0),
				NI_CtrInternalOutput(1),
				NI_AI_SampleClock,
				NI_AI_StartTrigger,
				NI_AI_ReferenceTrigger,
				NI_AI_ConvertClock,
				NI_AO_SampleClock,
				NI_AO_StartTrigger,
				NI_DI_SampleClock,
				NI_DO_SampleClock,
				NI_FrequencyOutput,
				NI_ChangeDetectionEvent,
				NI_AnalogComparisonEvent,
				0, /* Termination */
			}
		},
		{
			.dest = NI_PFI(9),
			.src = (int[]){
				TRIGGER_LINE(0),
				TRIGGER_LINE(1),
				TRIGGER_LINE(2),
				TRIGGER_LINE(3),
				TRIGGER_LINE(4),
				TRIGGER_LINE(5),
				TRIGGER_LINE(6),
				TRIGGER_LINE(7),
				NI_CtrSource(0),
				NI_CtrSource(1),
				NI_CtrGate(0),
				NI_CtrGate(1),
				NI_CtrInternalOutput(0),
				NI_CtrInternalOutput(1),
				NI_AI_SampleClock,
				NI_AI_StartTrigger,
				NI_AI_ReferenceTrigger,
				NI_AI_ConvertClock,
				NI_AO_SampleClock,
				NI_AO_StartTrigger,
				NI_DI_SampleClock,
				NI_DO_SampleClock,
				NI_FrequencyOutput,
				NI_ChangeDetectionEvent,
				NI_AnalogComparisonEvent,
				0, /* Termination */
			}
		},
		{
			.dest = NI_PFI(10),
			.src = (int[]){
				TRIGGER_LINE(0),
				TRIGGER_LINE(1),
				TRIGGER_LINE(2),
				TRIGGER_LINE(3),
				TRIGGER_LINE(4),
				TRIGGER_LINE(5),
				TRIGGER_LINE(6),
				TRIGGER_LINE(7),
				NI_CtrSource(0),
				NI_CtrSource(1),
				NI_CtrGate(0),
				NI_CtrGate(1),
				NI_CtrInternalOutput(0),
				NI_CtrInternalOutput(1),
				NI_AI_SampleClock,
				NI_AI_StartTrigger,
				NI_AI_ReferenceTrigger,
				NI_AI_ConvertClock,
				NI_AO_SampleClock,
				NI_AO_StartTrigger,
				NI_DI_SampleClock,
				NI_DO_SampleClock,
				NI_FrequencyOutput,
				NI_ChangeDetectionEvent,
				NI_AnalogComparisonEvent,
				0, /* Termination */
			}
		},
		{
			.dest = NI_PFI(11),
			.src = (int[]){
				TRIGGER_LINE(0),
				TRIGGER_LINE(1),
				TRIGGER_LINE(2),
				TRIGGER_LINE(3),
				TRIGGER_LINE(4),
				TRIGGER_LINE(5),
				TRIGGER_LINE(6),
				TRIGGER_LINE(7),
				NI_CtrSource(0),
				NI_CtrSource(1),
				NI_CtrGate(0),
				NI_CtrGate(1),
				NI_CtrInternalOutput(0),
				NI_CtrInternalOutput(1),
				NI_AI_SampleClock,
				NI_AI_StartTrigger,
				NI_AI_ReferenceTrigger,
				NI_AI_ConvertClock,
				NI_AO_SampleClock,
				NI_AO_StartTrigger,
				NI_DI_SampleClock,
				NI_DO_SampleClock,
				NI_FrequencyOutput,
				NI_ChangeDetectionEvent,
				NI_AnalogComparisonEvent,
				0, /* Termination */
			}
		},
		{
			.dest = NI_PFI(12),
			.src = (int[]){
				TRIGGER_LINE(0),
				TRIGGER_LINE(1),
				TRIGGER_LINE(2),
				TRIGGER_LINE(3),
				TRIGGER_LINE(4),
				TRIGGER_LINE(5),
				TRIGGER_LINE(6),
				TRIGGER_LINE(7),
				NI_CtrSource(0),
				NI_CtrSource(1),
				NI_CtrGate(0),
				NI_CtrGate(1),
				NI_CtrInternalOutput(0),
				NI_CtrInternalOutput(1),
				NI_AI_SampleClock,
				NI_AI_StartTrigger,
				NI_AI_ReferenceTrigger,
				NI_AI_ConvertClock,
				NI_AO_SampleClock,
				NI_AO_StartTrigger,
				NI_DI_SampleClock,
				NI_DO_SampleClock,
				NI_FrequencyOutput,
				NI_ChangeDetectionEvent,
				NI_AnalogComparisonEvent,
				0, /* Termination */
			}
		},
		{
			.dest = NI_PFI(13),
			.src = (int[]){
				TRIGGER_LINE(0),
				TRIGGER_LINE(1),
				TRIGGER_LINE(2),
				TRIGGER_LINE(3),
				TRIGGER_LINE(4),
				TRIGGER_LINE(5),
				TRIGGER_LINE(6),
				TRIGGER_LINE(7),
				NI_CtrSource(0),
				NI_CtrSource(1),
				NI_CtrGate(0),
				NI_CtrGate(1),
				NI_CtrInternalOutput(0),
				NI_CtrInternalOutput(1),
				NI_AI_SampleClock,
				NI_AI_StartTrigger,
				NI_AI_ReferenceTrigger,
				NI_AI_ConvertClock,
				NI_AO_SampleClock,
				NI_AO_StartTrigger,
				NI_DI_SampleClock,
				NI_DO_SampleClock,
				NI_FrequencyOutput,
				NI_ChangeDetectionEvent,
				NI_AnalogComparisonEvent,
				0, /* Termination */
			}
		},
		{
			.dest = NI_PFI(14),
			.src = (int[]){
				TRIGGER_LINE(0),
				TRIGGER_LINE(1),
				TRIGGER_LINE(2),
				TRIGGER_LINE(3),
				TRIGGER_LINE(4),
				TRIGGER_LINE(5),
				TRIGGER_LINE(6),
				TRIGGER_LINE(7),
				NI_CtrSource(0),
				NI_CtrSource(1),
				NI_CtrGate(0),
				NI_CtrGate(1),
				NI_CtrInternalOutput(0),
				NI_CtrInternalOutput(1),
				NI_AI_SampleClock,
				NI_AI_StartTrigger,
				NI_AI_ReferenceTrigger,
				NI_AI_ConvertClock,
				NI_AO_SampleClock,
				NI_AO_StartTrigger,
				NI_DI_SampleClock,
				NI_DO_SampleClock,
				NI_FrequencyOutput,
				NI_ChangeDetectionEvent,
				NI_AnalogComparisonEvent,
				0, /* Termination */
			}
		},
		{
			.dest = NI_PFI(15),
			.src = (int[]){
				TRIGGER_LINE(0),
				TRIGGER_LINE(1),
				TRIGGER_LINE(2),
				TRIGGER_LINE(3),
				TRIGGER_LINE(4),
				TRIGGER_LINE(5),
				TRIGGER_LINE(6),
				TRIGGER_LINE(7),
				NI_CtrSource(0),
				NI_CtrSource(1),
				NI_CtrGate(0),
				NI_CtrGate(1),
				NI_CtrInternalOutput(0),
				NI_CtrInternalOutput(1),
				NI_AI_SampleClock,
				NI_AI_StartTrigger,
				NI_AI_ReferenceTrigger,
				NI_AI_ConvertClock,
				NI_AO_SampleClock,
				NI_AO_StartTrigger,
				NI_DI_SampleClock,
				NI_DO_SampleClock,
				NI_FrequencyOutput,
				NI_ChangeDetectionEvent,
				NI_AnalogComparisonEvent,
				0, /* Termination */
			}
		},
		{
			.dest = TRIGGER_LINE(0),
			.src = (int[]){
				NI_PFI(0),
				NI_PFI(1),
				NI_PFI(2),
				NI_PFI(3),
				NI_PFI(4),
				NI_PFI(5),
				NI_CtrSource(0),
				NI_CtrSource(1),
				NI_CtrGate(0),
				NI_CtrGate(1),
				NI_CtrInternalOutput(0),
				NI_CtrInternalOutput(1),
				NI_AI_SampleClock,
				NI_AI_StartTrigger,
				NI_AI_ReferenceTrigger,
				NI_AI_ConvertClock,
				NI_AI_PauseTrigger,
				NI_AO_SampleClock,
				NI_AO_StartTrigger,
				NI_AO_PauseTrigger,
				NI_10MHzRefClock,
				NI_FrequencyOutput,
				NI_ChangeDetectionEvent,
				NI_AnalogComparisonEvent,
				0, /* Termination */
			}
		},
		{
			.dest = TRIGGER_LINE(1),
			.src = (int[]){
				NI_PFI(0),
				NI_PFI(1),
				NI_PFI(2),
				NI_PFI(3),
				NI_PFI(4),
				NI_PFI(5),
				NI_CtrSource(0),
				NI_CtrSource(1),
				NI_CtrGate(0),
				NI_CtrGate(1),
				NI_CtrInternalOutput(0),
				NI_CtrInternalOutput(1),
				NI_AI_SampleClock,
				NI_AI_StartTrigger,
				NI_AI_ReferenceTrigger,
				NI_AI_ConvertClock,
				NI_AI_PauseTrigger,
				NI_AO_SampleClock,
				NI_AO_StartTrigger,
				NI_AO_PauseTrigger,
				NI_10MHzRefClock,
				NI_FrequencyOutput,
				NI_ChangeDetectionEvent,
				NI_AnalogComparisonEvent,
				0, /* Termination */
			}
		},
		{
			.dest = TRIGGER_LINE(2),
			.src = (int[]){
				NI_PFI(0),
				NI_PFI(1),
				NI_PFI(2),
				NI_PFI(3),
				NI_PFI(4),
				NI_PFI(5),
				NI_CtrSource(0),
				NI_CtrSource(1),
				NI_CtrGate(0),
				NI_CtrGate(1),
				NI_CtrInternalOutput(0),
				NI_CtrInternalOutput(1),
				NI_AI_SampleClock,
				NI_AI_StartTrigger,
				NI_AI_ReferenceTrigger,
				NI_AI_ConvertClock,
				NI_AI_PauseTrigger,
				NI_AO_SampleClock,
				NI_AO_StartTrigger,
				NI_AO_PauseTrigger,
				NI_10MHzRefClock,
				NI_FrequencyOutput,
				NI_ChangeDetectionEvent,
				NI_AnalogComparisonEvent,
				0, /* Termination */
			}
		},
		{
			.dest = TRIGGER_LINE(3),
			.src = (int[]){
				NI_PFI(0),
				NI_PFI(1),
				NI_PFI(2),
				NI_PFI(3),
				NI_PFI(4),
				NI_PFI(5),
				NI_CtrSource(0),
				NI_CtrSource(1),
				NI_CtrGate(0),
				NI_CtrGate(1),
				NI_CtrInternalOutput(0),
				NI_CtrInternalOutput(1),
				NI_AI_SampleClock,
				NI_AI_StartTrigger,
				NI_AI_ReferenceTrigger,
				NI_AI_ConvertClock,
				NI_AI_PauseTrigger,
				NI_AO_SampleClock,
				NI_AO_StartTrigger,
				NI_AO_PauseTrigger,
				NI_10MHzRefClock,
				NI_FrequencyOutput,
				NI_ChangeDetectionEvent,
				NI_AnalogComparisonEvent,
				0, /* Termination */
			}
		},
		{
			.dest = TRIGGER_LINE(4),
			.src = (int[]){
				NI_PFI(0),
				NI_PFI(1),
				NI_PFI(2),
				NI_PFI(3),
				NI_PFI(4),
				NI_PFI(5),
				NI_CtrSource(0),
				NI_CtrSource(1),
				NI_CtrGate(0),
				NI_CtrGate(1),
				NI_CtrInternalOutput(0),
				NI_CtrInternalOutput(1),
				NI_AI_SampleClock,
				NI_AI_StartTrigger,
				NI_AI_ReferenceTrigger,
				NI_AI_ConvertClock,
				NI_AI_PauseTrigger,
				NI_AO_SampleClock,
				NI_AO_StartTrigger,
				NI_AO_PauseTrigger,
				NI_10MHzRefClock,
				NI_FrequencyOutput,
				NI_ChangeDetectionEvent,
				NI_AnalogComparisonEvent,
				0, /* Termination */
			}
		},
		{
			.dest = TRIGGER_LINE(5),
			.src = (int[]){
				NI_PFI(0),
				NI_PFI(1),
				NI_PFI(2),
				NI_PFI(3),
				NI_PFI(4),
				NI_PFI(5),
				NI_CtrSource(0),
				NI_CtrSource(1),
				NI_CtrGate(0),
				NI_CtrGate(1),
				NI_CtrInternalOutput(0),
				NI_CtrInternalOutput(1),
				NI_AI_SampleClock,
				NI_AI_StartTrigger,
				NI_AI_ReferenceTrigger,
				NI_AI_ConvertClock,
				NI_AI_PauseTrigger,
				NI_AO_SampleClock,
				NI_AO_StartTrigger,
				NI_AO_PauseTrigger,
				NI_10MHzRefClock,
				NI_FrequencyOutput,
				NI_ChangeDetectionEvent,
				NI_AnalogComparisonEvent,
				0, /* Termination */
			}
		},
		{
			.dest = TRIGGER_LINE(6),
			.src = (int[]){
				NI_PFI(0),
				NI_PFI(1),
				NI_PFI(2),
				NI_PFI(3),
				NI_PFI(4),
				NI_PFI(5),
				NI_CtrSource(0),
				NI_CtrSource(1),
				NI_CtrGate(0),
				NI_CtrGate(1),
				NI_CtrInternalOutput(0),
				NI_CtrInternalOutput(1),
				NI_AI_SampleClock,
				NI_AI_StartTrigger,
				NI_AI_ReferenceTrigger,
				NI_AI_ConvertClock,
				NI_AI_PauseTrigger,
				NI_AO_SampleClock,
				NI_AO_StartTrigger,
				NI_AO_PauseTrigger,
				NI_10MHzRefClock,
				NI_FrequencyOutput,
				NI_ChangeDetectionEvent,
				NI_AnalogComparisonEvent,
				0, /* Termination */
			}
		},
		{
			.dest = TRIGGER_LINE(7),
			.src = (int[]){
				NI_PFI(0),
				NI_PFI(1),
				NI_PFI(2),
				NI_PFI(3),
				NI_PFI(4),
				NI_PFI(5),
				NI_CtrSource(0),
				NI_CtrSource(1),
				NI_CtrGate(0),
				NI_CtrGate(1),
				NI_CtrInternalOutput(0),
				NI_CtrInternalOutput(1),
				NI_AI_SampleClock,
				NI_AI_StartTrigger,
				NI_AI_ReferenceTrigger,
				NI_AI_ConvertClock,
				NI_AI_PauseTrigger,
				NI_AO_SampleClock,
				NI_AO_StartTrigger,
				NI_AO_PauseTrigger,
				NI_10MHzRefClock,
				NI_FrequencyOutput,
				NI_ChangeDetectionEvent,
				NI_AnalogComparisonEvent,
				0, /* Termination */
			}
		},
		{
			.dest = NI_CtrSource(0),
			.src = (int[]){
				NI_PFI(0),
				NI_PFI(1),
				NI_PFI(2),
				NI_PFI(3),
				NI_PFI(4),
				NI_PFI(5),
				NI_PFI(6),
				NI_PFI(7),
				NI_PFI(8),
				NI_PFI(9),
				NI_PFI(10),
				NI_PFI(11),
				NI_PFI(12),
				NI_PFI(13),
				NI_PFI(14),
				NI_PFI(15),
				TRIGGER_LINE(0),
				TRIGGER_LINE(1),
				TRIGGER_LINE(2),
				TRIGGER_LINE(3),
				TRIGGER_LINE(4),
				TRIGGER_LINE(5),
				TRIGGER_LINE(6),
				TRIGGER_LINE(7),
				NI_CtrGate(1),
				NI_20MHzTimebase,
				NI_80MHzTimebase,
				NI_100kHzTimebase,
				NI_AnalogComparisonEvent,
				0, /* Termination */
			}
		},
		{
			.dest = NI_CtrSource(1),
			.src = (int[]){
				NI_PFI(0),
				NI_PFI(1),
				NI_PFI(2),
				NI_PFI(3),
				NI_PFI(4),
				NI_PFI(5),
				NI_PFI(6),
				NI_PFI(7),
				NI_PFI(8),
				NI_PFI(9),
				NI_PFI(10),
				NI_PFI(11),
				NI_PFI(12),
				NI_PFI(13),
				NI_PFI(14),
				NI_PFI(15),
				TRIGGER_LINE(0),
				TRIGGER_LINE(1),
				TRIGGER_LINE(2),
				TRIGGER_LINE(3),
				TRIGGER_LINE(4),
				TRIGGER_LINE(5),
				TRIGGER_LINE(6),
				TRIGGER_LINE(7),
				NI_CtrGate(0),
				NI_20MHzTimebase,
				NI_80MHzTimebase,
				NI_100kHzTimebase,
				NI_AnalogComparisonEvent,
				0, /* Termination */
			}
		},
		{
			.dest = NI_CtrGate(0),
			.src = (int[]){
				NI_PFI(0),
				NI_PFI(1),
				NI_PFI(2),
				NI_PFI(3),
				NI_PFI(4),
				NI_PFI(5),
				NI_PFI(6),
				NI_PFI(7),
				NI_PFI(8),
				NI_PFI(9),
				NI_PFI(10),
				NI_PFI(11),
				NI_PFI(12),
				NI_PFI(13),
				NI_PFI(14),
				NI_PFI(15),
				TRIGGER_LINE(0),
				TRIGGER_LINE(1),
				TRIGGER_LINE(2),
				TRIGGER_LINE(3),
				TRIGGER_LINE(4),
				TRIGGER_LINE(5),
				TRIGGER_LINE(6),
				TRIGGER_LINE(7),
				NI_CtrSource(1),
				NI_CtrInternalOutput(1),
				NI_AI_StartTrigger,
				NI_AI_ReferenceTrigger,
				NI_AnalogComparisonEvent,
				0, /* Termination */
			}
		},
		{
			.dest = NI_CtrGate(1),
			.src = (int[]){
				NI_PFI(0),
				NI_PFI(1),
				NI_PFI(2),
				NI_PFI(3),
				NI_PFI(4),
				NI_PFI(5),
				NI_PFI(6),
				NI_PFI(7),
				NI_PFI(8),
				NI_PFI(9),
				NI_PFI(10),
				NI_PFI(11),
				NI_PFI(12),
				NI_PFI(13),
				NI_PFI(14),
				NI_PFI(15),
				TRIGGER_LINE(0),
				TRIGGER_LINE(1),
				TRIGGER_LINE(2),
				TRIGGER_LINE(3),
				TRIGGER_LINE(4),
				TRIGGER_LINE(5),
				TRIGGER_LINE(6),
				TRIGGER_LINE(7),
				NI_CtrSource(0),
				NI_CtrInternalOutput(0),
				NI_AI_StartTrigger,
				NI_AI_ReferenceTrigger,
				NI_AnalogComparisonEvent,
				0, /* Termination */
			}
		},
		{
			.dest = NI_CtrAux(0),
			.src = (int[]){
				NI_PFI(0),
				NI_PFI(1),
				NI_PFI(2),
				NI_PFI(3),
				NI_PFI(4),
				NI_PFI(5),
				NI_PFI(6),
				NI_PFI(7),
				NI_PFI(8),
				NI_PFI(9),
				NI_PFI(10),
				NI_PFI(11),
				NI_PFI(12),
				NI_PFI(13),
				NI_PFI(14),
				NI_PFI(15),
				TRIGGER_LINE(0),
				TRIGGER_LINE(1),
				TRIGGER_LINE(2),
				TRIGGER_LINE(3),
				TRIGGER_LINE(4),
				TRIGGER_LINE(5),
				TRIGGER_LINE(6),
				TRIGGER_LINE(7),
				NI_CtrSource(1),
				NI_CtrGate(0),
				NI_CtrGate(1),
				NI_CtrInternalOutput(1),
				NI_AI_StartTrigger,
				NI_AI_ReferenceTrigger,
				NI_AnalogComparisonEvent,
				0, /* Termination */
			}
		},
		{
			.dest = NI_CtrAux(1),
			.src = (int[]){
				NI_PFI(0),
				NI_PFI(1),
				NI_PFI(2),
				NI_PFI(3),
				NI_PFI(4),
				NI_PFI(5),
				NI_PFI(6),
				NI_PFI(7),
				NI_PFI(8),
				NI_PFI(9),
				NI_PFI(10),
				NI_PFI(11),
				NI_PFI(12),
				NI_PFI(13),
				NI_PFI(14),
				NI_PFI(15),
				TRIGGER_LINE(0),
				TRIGGER_LINE(1),
				TRIGGER_LINE(2),
				TRIGGER_LINE(3),
				TRIGGER_LINE(4),
				TRIGGER_LINE(5),
				TRIGGER_LINE(6),
				TRIGGER_LINE(7),
				NI_CtrSource(0),
				NI_CtrGate(0),
				NI_CtrGate(1),
				NI_CtrInternalOutput(0),
				NI_AI_StartTrigger,
				NI_AI_ReferenceTrigger,
				NI_AnalogComparisonEvent,
				0, /* Termination */
			}
		},
		{
			.dest = NI_CtrA(0),
			.src = (int[]){
				NI_PFI(0),
				NI_PFI(1),
				NI_PFI(2),
				NI_PFI(3),
				NI_PFI(4),
				NI_PFI(5),
				NI_PFI(6),
				NI_PFI(7),
				NI_PFI(8),
				NI_PFI(9),
				NI_PFI(10),
				NI_PFI(11),
				NI_PFI(12),
				NI_PFI(13),
				NI_PFI(14),
				NI_PFI(15),
				TRIGGER_LINE(0),
				TRIGGER_LINE(1),
				TRIGGER_LINE(2),
				TRIGGER_LINE(3),
				TRIGGER_LINE(4),
				TRIGGER_LINE(5),
				TRIGGER_LINE(6),
				TRIGGER_LINE(7),
				NI_AnalogComparisonEvent,
				0, /* Termination */
			}
		},
		{
			.dest = NI_CtrA(1),
			.src = (int[]){
				NI_PFI(0),
				NI_PFI(1),
				NI_PFI(2),
				NI_PFI(3),
				NI_PFI(4),
				NI_PFI(5),
				NI_PFI(6),
				NI_PFI(7),
				NI_PFI(8),
				NI_PFI(9),
				NI_PFI(10),
				NI_PFI(11),
				NI_PFI(12),
				NI_PFI(13),
				NI_PFI(14),
				NI_PFI(15),
				TRIGGER_LINE(0),
				TRIGGER_LINE(1),
				TRIGGER_LINE(2),
				TRIGGER_LINE(3),
				TRIGGER_LINE(4),
				TRIGGER_LINE(5),
				TRIGGER_LINE(6),
				TRIGGER_LINE(7),
				NI_AnalogComparisonEvent,
				0, /* Termination */
			}
		},
		{
			.dest = NI_CtrB(0),
			.src = (int[]){
				NI_PFI(0),
				NI_PFI(1),
				NI_PFI(2),
				NI_PFI(3),
				NI_PFI(4),
				NI_PFI(5),
				NI_PFI(6),
				NI_PFI(7),
				NI_PFI(8),
				NI_PFI(9),
				NI_PFI(10),
				NI_PFI(11),
				NI_PFI(12),
				NI_PFI(13),
				NI_PFI(14),
				NI_PFI(15),
				TRIGGER_LINE(0),
				TRIGGER_LINE(1),
				TRIGGER_LINE(2),
				TRIGGER_LINE(3),
				TRIGGER_LINE(4),
				TRIGGER_LINE(5),
				TRIGGER_LINE(6),
				TRIGGER_LINE(7),
				NI_AnalogComparisonEvent,
				0, /* Termination */
			}
		},
		{
			.dest = NI_CtrB(1),
			.src = (int[]){
				NI_PFI(0),
				NI_PFI(1),
				NI_PFI(2),
				NI_PFI(3),
				NI_PFI(4),
				NI_PFI(5),
				NI_PFI(6),
				NI_PFI(7),
				NI_PFI(8),
				NI_PFI(9),
				NI_PFI(10),
				NI_PFI(11),
				NI_PFI(12),
				NI_PFI(13),
				NI_PFI(14),
				NI_PFI(15),
				TRIGGER_LINE(0),
				TRIGGER_LINE(1),
				TRIGGER_LINE(2),
				TRIGGER_LINE(3),
				TRIGGER_LINE(4),
				TRIGGER_LINE(5),
				TRIGGER_LINE(6),
				TRIGGER_LINE(7),
				NI_AnalogComparisonEvent,
				0, /* Termination */
			}
		},
		{
			.dest = NI_CtrZ(0),
			.src = (int[]){
				NI_PFI(0),
				NI_PFI(1),
				NI_PFI(2),
				NI_PFI(3),
				NI_PFI(4),
				NI_PFI(5),
				NI_PFI(6),
				NI_PFI(7),
				NI_PFI(8),
				NI_PFI(9),
				NI_PFI(10),
				NI_PFI(11),
				NI_PFI(12),
				NI_PFI(13),
				NI_PFI(14),
				NI_PFI(15),
				TRIGGER_LINE(0),
				TRIGGER_LINE(1),
				TRIGGER_LINE(2),
				TRIGGER_LINE(3),
				TRIGGER_LINE(4),
				TRIGGER_LINE(5),
				TRIGGER_LINE(6),
				TRIGGER_LINE(7),
				NI_AnalogComparisonEvent,
				0, /* Termination */
			}
		},
		{
			.dest = NI_CtrZ(1),
			.src = (int[]){
				NI_PFI(0),
				NI_PFI(1),
				NI_PFI(2),
				NI_PFI(3),
				NI_PFI(4),
				NI_PFI(5),
				NI_PFI(6),
				NI_PFI(7),
				NI_PFI(8),
				NI_PFI(9),
				NI_PFI(10),
				NI_PFI(11),
				NI_PFI(12),
				NI_PFI(13),
				NI_PFI(14),
				NI_PFI(15),
				TRIGGER_LINE(0),
				TRIGGER_LINE(1),
				TRIGGER_LINE(2),
				TRIGGER_LINE(3),
				TRIGGER_LINE(4),
				TRIGGER_LINE(5),
				TRIGGER_LINE(6),
				TRIGGER_LINE(7),
				NI_AnalogComparisonEvent,
				0, /* Termination */
			}
		},
		{
			.dest = NI_CtrArmStartTrigger(0),
			.src = (int[]){
				NI_PFI(0),
				NI_PFI(1),
				NI_PFI(2),
				NI_PFI(3),
				NI_PFI(4),
				NI_PFI(5),
				NI_PFI(6),
				NI_PFI(7),
				NI_PFI(8),
				NI_PFI(9),
				NI_PFI(10),
				NI_PFI(11),
				NI_PFI(12),
				NI_PFI(13),
				NI_PFI(14),
				NI_PFI(15),
				TRIGGER_LINE(0),
				TRIGGER_LINE(1),
				TRIGGER_LINE(2),
				TRIGGER_LINE(3),
				TRIGGER_LINE(4),
				TRIGGER_LINE(5),
				TRIGGER_LINE(6),
				TRIGGER_LINE(7),
				NI_CtrInternalOutput(1),
				NI_AI_StartTrigger,
				NI_AI_ReferenceTrigger,
				NI_AnalogComparisonEvent,
				0, /* Termination */
			}
		},
		{
			.dest = NI_CtrArmStartTrigger(1),
			.src = (int[]){
				NI_PFI(0),
				NI_PFI(1),
				NI_PFI(2),
				NI_PFI(3),
				NI_PFI(4),
				NI_PFI(5),
				NI_PFI(6),
				NI_PFI(7),
				NI_PFI(8),
				NI_PFI(9),
				NI_PFI(10),
				NI_PFI(11),
				NI_PFI(12),
				NI_PFI(13),
				NI_PFI(14),
				NI_PFI(15),
				TRIGGER_LINE(0),
				TRIGGER_LINE(1),
				TRIGGER_LINE(2),
				TRIGGER_LINE(3),
				TRIGGER_LINE(4),
				TRIGGER_LINE(5),
				TRIGGER_LINE(6),
				TRIGGER_LINE(7),
				NI_CtrInternalOutput(0),
				NI_AI_StartTrigger,
				NI_AI_ReferenceTrigger,
				NI_AnalogComparisonEvent,
				0, /* Termination */
			}
		},
		{
			.dest = NI_AI_SampleClock,
			.src = (int[]){
				NI_PFI(0),
				NI_PFI(1),
				NI_PFI(2),
				NI_PFI(3),
				NI_PFI(4),
				NI_PFI(5),
				NI_PFI(6),
				NI_PFI(7),
				NI_PFI(8),
				NI_PFI(9),
				NI_PFI(10),
				NI_PFI(11),
				NI_PFI(12),
				NI_PFI(13),
				NI_PFI(14),
				NI_PFI(15),
				TRIGGER_LINE(0),
				TRIGGER_LINE(1),
				TRIGGER_LINE(2),
				TRIGGER_LINE(3),
				TRIGGER_LINE(4),
				TRIGGER_LINE(5),
				TRIGGER_LINE(6),
				TRIGGER_LINE(7),
				NI_CtrInternalOutput(0),
				NI_CtrInternalOutput(1),
				NI_AI_SampleClockTimebase,
				NI_AnalogComparisonEvent,
				0, /* Termination */
			}
		},
		{
			.dest = NI_AI_SampleClockTimebase,
			.src = (int[]){
				NI_PFI(0),
				NI_PFI(1),
				NI_PFI(2),
				NI_PFI(3),
				NI_PFI(4),
				NI_PFI(5),
				NI_PFI(6),
				NI_PFI(7),
				NI_PFI(8),
				NI_PFI(9),
				NI_PFI(10),
				NI_PFI(11),
				NI_PFI(12),
				NI_PFI(13),
				NI_PFI(14),
				NI_PFI(15),
				TRIGGER_LINE(0),
				TRIGGER_LINE(1),
				TRIGGER_LINE(2),
				TRIGGER_LINE(3),
				TRIGGER_LINE(4),
				TRIGGER_LINE(5),
				TRIGGER_LINE(6),
				TRIGGER_LINE(7),
				NI_20MHzTimebase,
				NI_100kHzTimebase,
				NI_AnalogComparisonEvent,
				0, /* Termination */
			}
		},
		{
			.dest = NI_AI_StartTrigger,
			.src = (int[]){
				NI_PFI(0),
				NI_PFI(1),
				NI_PFI(2),
				NI_PFI(3),
				NI_PFI(4),
				NI_PFI(5),
				NI_PFI(6),
				NI_PFI(7),
				NI_PFI(8),
				NI_PFI(9),
				NI_PFI(10),
				NI_PFI(11),
				NI_PFI(12),
				NI_PFI(13),
				NI_PFI(14),
				NI_PFI(15),
				TRIGGER_LINE(0),
				TRIGGER_LINE(1),
				TRIGGER_LINE(2),
				TRIGGER_LINE(3),
				TRIGGER_LINE(4),
				TRIGGER_LINE(5),
				TRIGGER_LINE(6),
				TRIGGER_LINE(7),
				NI_CtrInternalOutput(0),
				NI_CtrInternalOutput(1),
				NI_AnalogComparisonEvent,
				0, /* Termination */
			}
		},
		{
			.dest = NI_AI_ReferenceTrigger,
			.src = (int[]){
				NI_PFI(0),
				NI_PFI(1),
				NI_PFI(2),
				NI_PFI(3),
				NI_PFI(4),
				NI_PFI(5),
				NI_PFI(6),
				NI_PFI(7),
				NI_PFI(8),
				NI_PFI(9),
				NI_PFI(10),
				NI_PFI(11),
				NI_PFI(12),
				NI_PFI(13),
				NI_PFI(14),
				NI_PFI(15),
				TRIGGER_LINE(0),
				TRIGGER_LINE(1),
				TRIGGER_LINE(2),
				TRIGGER_LINE(3),
				TRIGGER_LINE(4),
				TRIGGER_LINE(5),
				TRIGGER_LINE(6),
				TRIGGER_LINE(7),
				NI_AnalogComparisonEvent,
				0, /* Termination */
			}
		},
		{
			.dest = NI_AI_ConvertClock,
			.src = (int[]){
				NI_PFI(0),
				NI_PFI(1),
				NI_PFI(2),
				NI_PFI(3),
				NI_PFI(4),
				NI_PFI(5),
				NI_PFI(6),
				NI_PFI(7),
				NI_PFI(8),
				NI_PFI(9),
				NI_PFI(10),
				NI_PFI(11),
				NI_PFI(12),
				NI_PFI(13),
				NI_PFI(14),
				NI_PFI(15),
				TRIGGER_LINE(0),
				TRIGGER_LINE(1),
				TRIGGER_LINE(2),
				TRIGGER_LINE(3),
				TRIGGER_LINE(4),
				TRIGGER_LINE(5),
				TRIGGER_LINE(6),
				TRIGGER_LINE(7),
				NI_CtrInternalOutput(0),
				NI_CtrInternalOutput(1),
				NI_AI_ConvertClockTimebase,
				NI_AnalogComparisonEvent,
				0, /* Termination */
			}
		},
		{
			.dest = NI_AI_ConvertClockTimebase,
			.src = (int[]){
				NI_AI_SampleClockTimebase,
				NI_20MHzTimebase,
				0, /* Termination */
			}
		},
		{
			.dest = NI_AI_PauseTrigger,
			.src = (int[]){
				NI_PFI(0),
				NI_PFI(1),
				NI_PFI(2),
				NI_PFI(3),
				NI_PFI(4),
				NI_PFI(5),
				NI_PFI(6),
				NI_PFI(7),
				NI_PFI(8),
				NI_PFI(9),
				NI_PFI(10),
				NI_PFI(11),
				NI_PFI(12),
				NI_PFI(13),
				NI_PFI(14),
				NI_PFI(15),
				TRIGGER_LINE(0),
				TRIGGER_LINE(1),
				TRIGGER_LINE(2),
				TRIGGER_LINE(3),
				TRIGGER_LINE(4),
				TRIGGER_LINE(5),
				TRIGGER_LINE(6),
				TRIGGER_LINE(7),
				NI_AnalogComparisonEvent,
				0, /* Termination */
			}
		},
		{
			.dest = NI_AO_SampleClock,
			.src = (int[]){
				NI_PFI(0),
				NI_PFI(1),
				NI_PFI(2),
				NI_PFI(3),
				NI_PFI(4),
				NI_PFI(5),
				NI_PFI(6),
				NI_PFI(7),
				NI_PFI(8),
				NI_PFI(9),
				NI_PFI(10),
				NI_PFI(11),
				NI_PFI(12),
				NI_PFI(13),
				NI_PFI(14),
				NI_PFI(15),
				TRIGGER_LINE(0),
				TRIGGER_LINE(1),
				TRIGGER_LINE(2),
				TRIGGER_LINE(3),
				TRIGGER_LINE(4),
				TRIGGER_LINE(5),
				TRIGGER_LINE(6),
				TRIGGER_LINE(7),
				NI_CtrInternalOutput(0),
				NI_CtrInternalOutput(1),
				NI_AO_SampleClockTimebase,
				NI_AnalogComparisonEvent,
				0, /* Termination */
			}
		},
		{
			.dest = NI_AO_SampleClockTimebase,
			.src = (int[]){
				NI_PFI(0),
				NI_PFI(1),
				NI_PFI(2),
				NI_PFI(3),
				NI_PFI(4),
				NI_PFI(5),
				NI_PFI(6),
				NI_PFI(7),
				NI_PFI(8),
				NI_PFI(9),
				NI_PFI(10),
				NI_PFI(11),
				NI_PFI(12),
				NI_PFI(13),
				NI_PFI(14),
				NI_PFI(15),
				TRIGGER_LINE(0),
				TRIGGER_LINE(1),
				TRIGGER_LINE(2),
				TRIGGER_LINE(3),
				TRIGGER_LINE(4),
				TRIGGER_LINE(5),
				TRIGGER_LINE(6),
				TRIGGER_LINE(7),
				NI_20MHzTimebase,
				NI_100kHzTimebase,
				NI_AnalogComparisonEvent,
				0, /* Termination */
			}
		},
		{
			.dest = NI_AO_StartTrigger,
			.src = (int[]){
				NI_PFI(0),
				NI_PFI(1),
				NI_PFI(2),
				NI_PFI(3),
				NI_PFI(4),
				NI_PFI(5),
				NI_PFI(6),
				NI_PFI(7),
				NI_PFI(8),
				NI_PFI(9),
				NI_PFI(10),
				NI_PFI(11),
				NI_PFI(12),
				NI_PFI(13),
				NI_PFI(14),
				NI_PFI(15),
				TRIGGER_LINE(0),
				TRIGGER_LINE(1),
				TRIGGER_LINE(2),
				TRIGGER_LINE(3),
				TRIGGER_LINE(4),
				TRIGGER_LINE(5),
				TRIGGER_LINE(6),
				TRIGGER_LINE(7),
				NI_AI_StartTrigger,
				NI_AnalogComparisonEvent,
				0, /* Termination */
			}
		},
		{
			.dest = NI_AO_PauseTrigger,
			.src = (int[]){
				NI_PFI(0),
				NI_PFI(1),
				NI_PFI(2),
				NI_PFI(3),
				NI_PFI(4),
				NI_PFI(5),
				NI_PFI(6),
				NI_PFI(7),
				NI_PFI(8),
				NI_PFI(9),
				NI_PFI(10),
				NI_PFI(11),
				NI_PFI(12),
				NI_PFI(13),
				NI_PFI(14),
				NI_PFI(15),
				TRIGGER_LINE(0),
				TRIGGER_LINE(1),
				TRIGGER_LINE(2),
				TRIGGER_LINE(3),
				TRIGGER_LINE(4),
				TRIGGER_LINE(5),
				TRIGGER_LINE(6),
				TRIGGER_LINE(7),
				NI_AnalogComparisonEvent,
				0, /* Termination */
			}
		},
		{
			.dest = NI_DI_SampleClock,
			.src = (int[]){
				NI_PFI(0),
				NI_PFI(1),
				NI_PFI(2),
				NI_PFI(3),
				NI_PFI(4),
				NI_PFI(5),
				NI_PFI(6),
				NI_PFI(7),
				NI_PFI(8),
				NI_PFI(9),
				NI_PFI(10),
				NI_PFI(11),
				NI_PFI(12),
				NI_PFI(13),
				NI_PFI(14),
				NI_PFI(15),
				TRIGGER_LINE(0),
				TRIGGER_LINE(1),
				TRIGGER_LINE(2),
				TRIGGER_LINE(3),
				TRIGGER_LINE(4),
				TRIGGER_LINE(5),
				TRIGGER_LINE(6),
				TRIGGER_LINE(7),
				NI_CtrInternalOutput(0),
				NI_CtrInternalOutput(1),
				NI_AI_SampleClock,
				NI_AI_ConvertClock,
				NI_AO_SampleClock,
				NI_FrequencyOutput,
				NI_ChangeDetectionEvent,
				NI_AnalogComparisonEvent,
				0, /* Termination */
			}
		},
		{
			.dest = NI_DO_SampleClock,
			.src = (int[]){
				NI_PFI(0),
				NI_PFI(1),
				NI_PFI(2),
				NI_PFI(3),
				NI_PFI(4),
				NI_PFI(5),
				NI_PFI(6),
				NI_PFI(7),
				NI_PFI(8),
				NI_PFI(9),
				NI_PFI(10),
				NI_PFI(11),
				NI_PFI(12),
				NI_PFI(13),
				NI_PFI(14),
				NI_PFI(15),
				TRIGGER_LINE(0),
				TRIGGER_LINE(1),
				TRIGGER_LINE(2),
				TRIGGER_LINE(3),
				TRIGGER_LINE(4),
				TRIGGER_LINE(5),
				TRIGGER_LINE(6),
				TRIGGER_LINE(7),
				NI_CtrInternalOutput(0),
				NI_CtrInternalOutput(1),
				NI_AI_SampleClock,
				NI_AI_ConvertClock,
				NI_AO_SampleClock,
				NI_FrequencyOutput,
				NI_ChangeDetectionEvent,
				NI_AnalogComparisonEvent,
				0, /* Termination */
			}
		},
		{ /* Termination of list */
			.dest = 0,
		},
	},
};
