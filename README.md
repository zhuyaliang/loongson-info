# loongson-info

## Compile and install

```
meson build -Dprefix=/usr

ninja -C build

sudo ninja -C build install

```

## Dbus 接口验证
```
dbus-send --system --print-reply --dest=cn.loongson.info /cn/loongson/info cn.loongson.info.方法
```
## 使用：
```
[test@localhost data]$ dbus-send --system --print-reply --dest=cn.loongson.info /cn/loongson/info cn.loongson.info.CpuName
method return time=1657701132.266672 sender=:1.2471 -> destination=:1.2479 serial=13 reply_serial=2
   string "3A5000"
```
## 方法列表
1. CpuName
2. PowerWaste
3. JunctionTemperature
4. PackagingMethod
5. CpuSizes
6. ProductName
7. BiosName
8. MemoryStyle
9. MicroArchitecture
10. CpuCache
11. MmuStyle
12. CalculationPart
13. ExtendedInstruction
14. HardwareAssistedVirtualization
15. KeyManagement
16. MemoryVerification
17. TrustedStart
18. CpuTemperature
19. PhysicalKernel
20. MaximumMemoryFrequency
21. MemoryChannel
22. MaximumMemoryCapacity
23. MaximumCpuFrequency
24. CpuThreads
25. FanSpeed
