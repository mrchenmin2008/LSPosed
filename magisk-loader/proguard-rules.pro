-keepclasseswithmembers class org.lsposed.lspd.core.Main {
    public static void forkCommon(boolean, java.lang.String, java.lang.String, android.os.IBinder);
}
-keepclasseswithmembers,includedescriptorclasses class * {
    native <methods>;
}
-keepclasseswithmembers class org.lsposed.lspd.service.BridgeService {
    public static boolean *(android.os.IBinder, int, long, long, int);
}

# 保留 pmxped 所有类和内部类
-keep class de.robv.android.pmxped.** { *; }
-keep class de.robv.android.pmxped.XC_MethodHook$* { *; }
-keep class de.robv.android.pmxped.XC_MethodReplacement$* { *; }

# 忽略警告
-dontwarn de.robv.android.pmxped.**

-assumenosideeffects class android.util.Log {
    public static *** v(...);
    public static *** d(...);
}
-repackageclasses
-allowaccessmodification
-dontwarn org.lsposed.lspd.core.*
-dontwarn org.lsposed.lspd.util.Hookers
