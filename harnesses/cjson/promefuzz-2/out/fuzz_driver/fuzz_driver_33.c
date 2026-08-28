// This fuzz driver is generated for library cjson, aiming to fuzz the following functions:
// cJSON_CreateNull at cJSON.c:2461:23 in cJSON.h
// cJSON_AddItemReferenceToArray at cJSON.c:2130:26 in cJSON.h
// cJSON_AddItemReferenceToArray at cJSON.c:2130:26 in cJSON.h
// cJSON_AddItemReferenceToObject at cJSON.c:2140:26 in cJSON.h
// cJSON_AddItemReferenceToObject at cJSON.c:2140:26 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_AddItemReferenceToArray at cJSON.c:2130:26 in cJSON.h
// cJSON_AddItemReferenceToObject at cJSON.c:2140:26 in cJSON.h
// cJSON_AddItemReferenceToObject at cJSON.c:2140:26 in cJSON.h
// cJSON_DetachItemViaPointer at cJSON.c:2258:23 in cJSON.h
// cJSON_DetachItemViaPointer at cJSON.c:2258:23 in cJSON.h
// cJSON_DetachItemViaPointer at cJSON.c:2258:23 in cJSON.h
// cJSON_AddItemToArray at cJSON.c:2061:26 in cJSON.h
// cJSON_AddItemToArray at cJSON.c:2061:26 in cJSON.h
// cJSON_AddItemToObjectCS at cJSON.c:2125:26 in cJSON.h
// cJSON_AddItemToObjectCS at cJSON.c:2125:26 in cJSON.h
// cJSON_CreateNull at cJSON.c:2461:23 in cJSON.h
// cJSON_DetachItemViaPointer at cJSON.c:2258:23 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_CreateArray at cJSON.c:2598:23 in cJSON.h
// cJSON_CreateArray at cJSON.c:2598:23 in cJSON.h
// cJSON_CreateObject at cJSON.c:2609:23 in cJSON.h
// cJSON_CreateObject at cJSON.c:2609:23 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_Delete at cJSON.c:253:20 in cJSON.h
// cJSON_CreateNull at cJSON.c:2461:23 in cJSON.h
// cJSON_AddItemToArray at cJSON.c:2061:26 in cJSON.h
// cJSON_AddItemToArray at cJSON.c:2061:26 in cJSON.h
// cJSON_AddItemToObjectCS at cJSON.c:2125:26 in cJSON.h
// cJSON_AddItemToObjectCS at cJSON.c:2125:26 in cJSON.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "cJSON.h"

static uint32_t read_u32(const uint8_t *Data, size_t Size, size_t *offset)
{
    uint32_t v = 0;
    size_t i;
    for (i = 0; i < 4 && *offset < Size; i++, (*offset)++)
    {
        v = (v << 8) | Data[*offset];
    }
    return v;
}

static size_t bounded_count(const uint8_t *Data, size_t Size, size_t *offset, size_t max_count)
{
    if (max_count == 0)
    {
        return 0;
    }
    return (size_t)(read_u32(Data, Size, offset) % (uint32_t)max_count);
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size)
{
    size_t offset = 0;
    size_t i, j;

    FILE *fp = fopen("./dummy_file", "wb");
    if (fp != NULL)
    {
        if (Size > 0)
        {
            (void)fwrite(Data, 1, Size, fp);
        }
        fclose(fp);
    }

    cJSON *array1 = cJSON_CreateArray();
    cJSON *array2 = cJSON_CreateArray();
    cJSON *object1 = cJSON_CreateObject();
    cJSON *object2 = cJSON_CreateObject();

    cJSON *pool[32];
    size_t pool_count = 0;

    if (array1 == NULL || array2 == NULL || object1 == NULL || object2 == NULL)
    {
        cJSON_Delete(array1);
        cJSON_Delete(array2);
        cJSON_Delete(object1);
        cJSON_Delete(object2);
        return 0;
    }

    {
        size_t create_count = bounded_count(Data, Size, &offset, 24) + 1;
        for (i = 0; i < create_count && pool_count < (sizeof(pool) / sizeof(pool[0])); i++)
        {
            cJSON *n = cJSON_CreateNull();
            if (n != NULL)
            {
                pool[pool_count++] = n;
            }
        }
    }

    for (i = 0; i < pool_count; i++)
    {
        uint8_t action = 0;
        if (offset < Size)
        {
            action = Data[offset++];
        }

        switch (action % 4)
        {
            case 0:
                cJSON_AddItemToArray(array1, pool[i]);
                break;
            case 1:
                cJSON_AddItemToArray(array2, pool[i]);
                break;
            case 2:
                cJSON_AddItemToObjectCS(object1, "const_key", pool[i]);
                break;
            default:
                cJSON_AddItemToObjectCS(object2, "other_const_key", pool[i]);
                break;
        }
    }

    {
        cJSON *extra = cJSON_CreateNull();
        if (extra != NULL)
        {
            (void)cJSON_AddItemReferenceToArray(array1, extra);
            (void)cJSON_AddItemReferenceToArray(array2, extra);
            (void)cJSON_AddItemReferenceToObject(object1, "ref_key", extra);
            (void)cJSON_AddItemReferenceToObject(object2, "ref_key2", extra);
            cJSON_Delete(extra);
        }
    }

    {
        cJSON *null_item = NULL;
        (void)cJSON_AddItemReferenceToArray(NULL, null_item);
        (void)cJSON_AddItemReferenceToObject(NULL, "x", null_item);
        (void)cJSON_AddItemReferenceToObject(object1, NULL, null_item);
        (void)cJSON_DetachItemViaPointer(NULL, NULL);
        (void)cJSON_DetachItemViaPointer(array1, NULL);
    }

    {
        size_t ops = bounded_count(Data, Size, &offset, 32);
        for (i = 0; i < ops; i++)
        {
            cJSON *parent = NULL;
            cJSON *child = NULL;
            uint8_t sel = 0;

            if (offset < Size)
            {
                sel = Data[offset++];
            }

            switch (sel % 4)
            {
                case 0:
                    parent = array1;
                    break;
                case 1:
                    parent = array2;
                    break;
                case 2:
                    parent = object1;
                    break;
                default:
                    parent = object2;
                    break;
            }

            if (parent != NULL && parent->child != NULL)
            {
                child = parent->child;
                if (offset < Size)
                {
                    size_t steps = Data[offset++] % 8;
                    for (j = 0; j < steps && child->next != NULL; j++)
                    {
                        child = child->next;
                    }
                }

                cJSON *detached = cJSON_DetachItemViaPointer(parent, child);
                if (detached != NULL)
                {
                    uint8_t reattach = 0;
                    if (offset < Size)
                    {
                        reattach = Data[offset++];
                    }

                    switch (reattach % 4)
                    {
                        case 0:
                            cJSON_AddItemToArray(array1, detached);
                            break;
                        case 1:
                            cJSON_AddItemToArray(array2, detached);
                            break;
                        case 2:
                            cJSON_AddItemToObjectCS(object1, "reattach_key", detached);
                            break;
                        default:
                            cJSON_AddItemToObjectCS(object2, "reattach_key2", detached);
                            break;
                    }
                }
            }
        }
    }

    {
        cJSON *standalone = cJSON_CreateNull();
        if (standalone != NULL)
        {
            (void)cJSON_DetachItemViaPointer(array1, standalone);
            cJSON_Delete(standalone);
        }
    }

    cJSON_Delete(array1);
    cJSON_Delete(array2);
    cJSON_Delete(object1);
    cJSON_Delete(object2);

    return 0;
}