#ifndef _PATHREMAPPING_H_
#define _PATHREMAPPING_H_

typedef size_t(*RemapPathFunction)(const char* path, char* buffer, size_t buffer_len);

void register_path_remapper (RemapPathFunction func);
const char* mono_unity_remap_path (const char* path);
void SetupMonoProcOverrides();

#endif