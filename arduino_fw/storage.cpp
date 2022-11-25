

#include <nvs_flash.h>

#define MCW_NS "tick-led"

uint8_t stInit = 0;
nvs_handle_t stHandle;
nvs_iterator_t stIt = NULL;

int storeInit(void) {
    int r;

    //already initialised
    if (stInit) {
        return 0;
    }

    r = nvs_flash_init();
    if (ESP_OK == r) {
        r = nvs_open(MCW_NS, NVS_READWRITE, &stHandle);
        if (ESP_OK == r) {
            stInit = 1;
        } else {
            nvs_flash_deinit();
        }
    }
    return r;
}

size_t storeGetRecordLen(const char* name) {
    if (NULL == name || (!stInit)) {
        return 0;
    }
    size_t len;
    if (ESP_OK == nvs_get_blob(stHandle, name, NULL, &len)) {
        return len;  
    }
    return 0;
}

bool storeSaveRecord(const char* name, uint8_t* record, size_t len) {
    esp_err_t r;
    if (NULL == name || NULL == record || len < 1 || (!stInit)) {
        return false;
    }
    r = nvs_set_blob(stHandle, name, record,  len);
    if (ESP_OK != r) {
        return false;
    }
    r  = nvs_commit(stHandle);
    return (ESP_OK == r);
}

bool storeLoadRecord(const char* name, uint8_t* record, size_t* len) {
    if (NULL == name || NULL == record || NULL == len || (!stInit)) {
        return false;
    }
    if (storeGetRecordLen(name) > *len) {
        return false;
    }
    return (ESP_OK == nvs_get_blob(stHandle, name, record, len));
}

bool storeEraseAll(void) {
    return (ESP_OK == nvs_erase_all(stHandle));
}



#if 0
// return true if a store record was found
bool storeListNext(nvs_entry_info_t* info) {
    if (NULL == info || !stInit) {
        return false;
    }
    
    if (NULL == stIt) {
        stIt = nvs_entry_find("nvs", MCW_NS, NVS_TYPE_ANY);
    } else {
        stIt = nvs_entry_next(stIt);
    }
    if (stIt) {
        nvs_entry_info(stIt, info);
        return true;
    }
    return false;
}



#endif

