# Basit Linux Karakter Cihaz Sürücüsü

Bu proje, Linux karakter cihaz sürücüsü (char device driver) yazımı için temel bir örnek sunar. Proje, cihaz dosyası üzerinden okuma ve yazma işlemlerini gerçekleştiren bir sürücü içerir. Bu çalışma, Linux kernel modülü geliştirme ile ilgilenen geliştiriciler için hazırlanmıştır.


---

## Özellikler

Okuma İşlevi: Cihazdan veri okunabilir.
Yazma İşlevi: Cihaza veri yazılabilir.
Loglama: dmesg ile cihazın kullanımı sırasında loglar görüntülenebilir.
Tampon Bellek: Kullanıcı verileri sürücünün tamponunda saklanır.



---

## Gereksinimler

Linux Kernel Headers (Kernel sürümünüze uygun başlık dosyaları)
GCC ve Make (Derleme araçları)



---

## Kurulum ve Kullanım

** 1. Kaynak Kodların Derlenmesi
Proje dizininde aşağıdaki komutu çalıştırarak modülü derleyin:

``` bash
make
```
Başarılı bir şekilde derlendikten sonra char_driver.ko dosyası oluşacaktır.

** 2. Kernel Modülünü Yükleme

Modülü Linux kerneline yüklemek için:

``` bash
sudo insmod char_driver.ko
```

** 3. Cihaz Dosyasını Oluşturma

dmesg komutu ile major numarasını öğrenin:

``` bash
dmesg | grep "char_device"
```

Cihaz dosyasını oluşturmak için:

``` bash
sudo mknod /dev/char_device c <major_number> 0
```

** 4. Cihaza Veri Yazma ve Okuma

Veri Yazma:

``` bash
echo "Merhaba Dünya" > /dev/char_device
```
Veri Okuma:

``` bash
cat /dev/char_device
```

** 5. Kernel Modülünü Kaldırma

Modülü sistemden kaldırmak için:

``` bash
sudo rmmod char_driver
```

Cihaz dosyasını silmek için:

``` bash
sudo rm /dev/char_device
```

---

## Dosya Yapısı

char_driver.c
Karakter cihaz sürücüsünün kaynak kodu. Temel okuma ve yazma işlevlerini içerir.

Makefile
Kernel modülünü derlemek için kullanılan Makefile.



---

## Loglama ve Takip

Cihaz sürücüsüyle ilgili detaylı loglar dmesg komutuyla izlenebilir:
``` bash
dmesg | grep char_device
```

---

## Örnek dmesg Çıktısı
``` bash
[12345.678901] char_device: Cihaz 1 kez açıldı.
[12345.678902] char_device: 13 bayt cihaza yazıldı.
[12345.678903] char_device: 13 bayt cihazdan okundu.
[12345.678904] char_device: Cihaz kapatıldı.
```



