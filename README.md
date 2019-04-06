# Using Toochain mipsel-openwrt-linux-uclibc-gcc build C Programs

## Sử dụng OpenWrtSDK build C/C++ Programs

LinkIt Smart 7688 chạy OpenWrt Linux hỗ trợ phát triển C/C++. Các chương trình được viết bằng C/C++ thường được gọi là ứng dụng gốc. Với ứng dụng gốc, có thể tạo trình điều khiển, ứng dụng hiệu suất tối ưu.

Để phát triển ứng dụng C/C++ yêu cầu toolchain để biên dịch và liên kết mã nguồn C/C++ thành các tệp thực thi binaries. Có thể cài đặt các công cụ phát triển trực tiếp trên môi trường LinkIt Smart 7688 Linux, nhưng với tài nguyên và bộ nhớ hạn chế chỉ 128MB RAM không đủ để sử dụng và điều này làm hạn chế cho phát triển ứng dụng C/C++ native. Vì vậy để phát triển ứng dụng C/C++ native cần cài đặt trình biên dịch chéo (cross-compile) trên máy tính để comiler ra chương trình thực thi trên Linkit Smart7688.

Cross-comile bao gồm gói phần mềm SDK LinkIt Smart 7688 hỗ trợ MacOSx và Ubuntu Linux, chưa hỗ trợ Windows.

## Cài đặt môi trường phát triển trên máy tính (host)

1. Download and unzip SKD package [Link Downloads](https://docs.labs.mediatek.com/resource/linkit-smart-7688/en/downloads)

    Unzip: SDK = tên file tải về
    sudo tar -xvjf SDK.tar.bz2

    cd SDK

2. Download and unzip [example file](https://labs.mediatek.com/en/download/kJWJnDta)

3. Copy file example helloworld directory to SDK/package folder. Cấu trúc folder như sau:
  ``` 
  SDK/package
   +helloworld              # Name of the package
        -Makefile           # This Makefile describes the package
        +src
            -Makefile       # This Makefile builds the binary
            -helloworld.c   # C/C++ source code
````

4. Trong thư mục SDK, enter make packge/helloworld/compile buil package:
   * Chuyển tới đường dẫn SDK/bin/ramips/packages/base.
   * tìm package tên helloworld_1.0.0-1_ramips_24kec.ipk.
   * Copy file .ipk tới LinkIt Smart 7688 board: scp ./tên_file.ipk root@mylinkit.local: 
    * SSH tới board LinkIt: ssh root@mylinkit.local, sau đó di chuyển tới đường dẫn chưa file .ipk và cài đặt: opkg install tenfile.ipk
    * Sau khi install xong, gõ helloworld sẽ xuất hiện chuỗi: Hello World, why won't my code compile? trên màn hình console.

5. Để biên dịch chéo các chương trình và thư viện hiện có, thực hiện:
```
    ./scripts/feeds update
    ./scripts/feeds list            # This gives you all the available pacakges
    ./scripts/feeds install curl    # for example we want to build curl
    make package/curl/compile 
```

## Tạo Makefile use Toolchain để build C Programs

Toolchain để  cross-compiler trên LinkIt7688 có tên: mipsel-openwrt-linux-uclibc-gcc có thể tìm thấy trong theo đường dẫn : "OpenWrt-SDK/staging_dir/toolchain-mipsel_24kec+dsp_gcc-4.8-linaro_uClibc-0.9.33.2/bin/" hoặc tải riêng toolchain [Link-toolchain for Ubuntu](https://labs.mediatek.com/en/download/2whWQCuB).

Makefile và cấu trúc thư mục được lưu trữ trong thư mục Projects:
1. helloworld: biên dịch đơn giản chương trình helloword chạy trên Linkit7688
2. mraa_blink: biên dịch chương trình blink_led GPIO44 chạy trên Linkit7688
3. projectTemplate: Makefile template tổ chức build chương trình phức tạp chia làm nhiều module file c. 
4. project build LCD1602 sử dụng thư viện mraa.
    ```
    make: build project
    make clean: làm sạch project (xóa các file object và file build)
    make flash: copy file .run qua LinkIt7688 bằng SSH
    ```