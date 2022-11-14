/*
 * Copyright (c) 2022, NXP
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 */

#ifndef DMA_NOTIFIER_H
#define DMA_NOTIFIER_H

#include "qom/object.h"
#include "qemu/notify.h"
#include "qemu/typedefs.h"

void dma_trigger_init_notifier_list(NotifierList * notifier_list);
void dma_trigger_notifier_connect(Notifier *notifier);
void dma_trigger_notify(void * data);
void dma_trigger_disconnect(Notifier *notifier);

#endif /* DMA_NOTIFIER_H */
