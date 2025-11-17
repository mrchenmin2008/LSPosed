/*
 * This file is part of LSPosed.
 *
 * LSPosed is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * LSPosed is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with LSPosed.  If not, see <https://www.gnu.org/licenses/>.
 *
 * Copyright (C) 2021 - 2022 LSPosed Contributors
 */

#include <dlfcn.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/socket.h>
#include <android/log.h>

#include "config_impl.h"
#include "magisk_loader.h"
#include "zygisk.hpp"

namespace lspd {
int allow_unload = 0;
int *allowUnload = &allow_unload;

class ZygiskModule : public zygisk::ModuleBase {
    JNIEnv *env_;
    zygisk::Api *api_;

    void onLoad(zygisk::Api *api, JNIEnv *env) override {
        env_ = env;
        api_ = api;
        MagiskLoader::Init();
        ConfigImpl::Init();
    }

    void preAppSpecialize(zygisk::AppSpecializeArgs *args) override {
        MagiskLoader::GetInstance()->OnNativeForkAndSpecializePre(
            env_, args->uid, args->gids, args->nice_name,
            args->is_child_zygote ? *args->is_child_zygote : false, args->app_data_dir);
    }

    void postAppSpecialize(const zygisk::AppSpecializeArgs *args) override {
        MagiskLoader::GetInstance()->OnNativeForkAndSpecializePost(env_, args->nice_name,
                                                                   args->app_data_dir);
        if (*allowUnload) api_->setOption(zygisk::DLCLOSE_MODULE_LIBRARY);
    }

    void preServerSpecialize([[maybe_unused]] zygisk::ServerSpecializeArgs *args) override {
        MagiskLoader::GetInstance()->OnNativeForkSystemServerPre(env_);
    }

    void postServerSpecialize([[maybe_unused]] const zygisk::ServerSpecializeArgs *args) override {
        if (__system_property_find("ro.vendor.product.ztename")) {
            auto *process = env_->FindClass("android/os/Process");
            auto *set_argv0 = env_->GetStaticMethodID(process, "setArgV0", "(Ljava/lang/String;)V");
            auto *name = env_->NewStringUTF("system_server");
            env_->CallStaticVoidMethod(process, set_argv0, name);
            env_->DeleteLocalRef(name);
            env_->DeleteLocalRef(process);
        }
        MagiskLoader::GetInstance()->OnNativeForkSystemServerPost(env_);
        if (*allowUnload) api_->setOption(zygisk::DLCLOSE_MODULE_LIBRARY);
    }
};
}  // namespace lspd

REGISTER_ZYGISK_MODULE(lspd::ZygiskModule);

//chenm
#define LSP_LOG_TAG "lpspace"
#define LSP_LOGI(...) __android_log_print(ANDROID_LOG_INFO, LSP_LOG_TAG, __VA_ARGS__)
#define LSP_LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LSP_LOG_TAG, __VA_ARGS__)
extern "C" {
using namespace lspd;
[[gnu::visibility("default")]]
void lpspace_init(JNIEnv* env) {
    (void) env;
    if (!env) {
        LSP_LOGE("lpspace_init: env == nullptr");
        return;
    }
    LSP_LOGI("lpspace_init: calling Init");
    MagiskLoader::Init();
    ConfigImpl::Init();

}


[[gnu::visibility("default")]]
void lpspace_SpecializePre(JNIEnv* env,
                                jint uid,
                                jintArray gids,
                                jstring nice_name,
                                jboolean is_child_zygote,
                                jstring app_data_dir) {

    LSP_LOGI("lpspace_SpecializePre--");
    MagiskLoader::GetInstance()->OnNativeForkAndSpecializePre(
            env, uid, gids, nice_name, (is_child_zygote == JNI_TRUE), app_data_dir);
}


[[gnu::visibility("default")]]
void lpspace_SpecializePost(JNIEnv* env,
                                 jstring nice_name,
                                 jstring app_data_dir) {
    LSP_LOGI("lpspace_SpecializePost---");
    MagiskLoader::GetInstance()->OnNativeForkAndSpecializePost(env, nice_name, app_data_dir);
}


[[gnu::visibility("default")]]
void lpspace_SystemServerPre(JNIEnv* env) {
    LSP_LOGI("lpspace_SystemServerPre-----");
    MagiskLoader::GetInstance()->OnNativeForkSystemServerPre(env);
}


[[gnu::visibility("default")]]
void lpspace_SystemServerPost(JNIEnv* env) {
    LSP_LOGI("lpspace_SystemServerPost----");
    MagiskLoader::GetInstance()->OnNativeForkSystemServerPost(env);
}

} // extern "C
//chenm