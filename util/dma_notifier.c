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

#include "qemu/osdep.h"
#include "qemu/notify.h"
#include "qemu/dma_notifier.h"


static NotifierList * dma_trigger_notifier_list = NULL;

void dma_trigger_init_notifier_list(NotifierList * notifier_list)
{
    notifier_list_init(notifier_list);
    dma_trigger_notifier_list = notifier_list;
}

void dma_trigger_notifier_connect(Notifier *notifier)
{
    if (dma_trigger_notifier_list != NULL)
    {
        notifier_list_add(dma_trigger_notifier_list, notifier);
    }
}

void dma_trigger_notify(void * data)
{
    if (dma_trigger_notifier_list != NULL)
    {
        notifier_list_notify(dma_trigger_notifier_list, data);
    }    
}

void dma_trigger_disconnect(Notifier *notifier)
{
    notifier_remove(notifier);
}


