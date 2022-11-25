#ifndef _STORAGE_H_
#define _STORAGE_H_


int storeInit(void);
size_t storeGetRecordLen(const char* name);
bool storeSaveRecord(const char* name, uint8_t* record, size_t len);
bool storeLoadRecord(const char* name, uint8_t* record, size_t* len);
bool storeEraseAll(void);


#if 0
// return true if a store record was found
bool storeListNext(nvs_entry_info_t* info);
#endif

#endif /* _STORAGE_H_ */
