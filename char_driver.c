#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>

#define DEVICE_NAME "char_device" // Cihaz adı
#define BUFFER_SIZE 1024          // Cihaz tampon boyutu

// Cihazın veri depolamak için tamponu
static char device_buffer[BUFFER_SIZE];
static int device_open_count = 0; // Cihazın kaç kez açıldığını takip eder
static dev_t dev_number;          // Major ve minor numaraları için cihaz numarası
static struct cdev char_device;   // Karakter cihaz yapısı

/**
 * @brief Cihaz açıldığında çağrılan fonksiyon.
  * 
   * inode ve file yapıları, cihaz hakkında bilgi sağlar.
    * Cihaz her açıldığında sayaç bir artırılır.
     */
     static int dev_open(struct inode *inode, struct file *file) {
         device_open_count++;
             printk(KERN_INFO "%s: Cihaz %d kez açıldı.\n", DEVICE_NAME, device_open_count);
                 return 0; // Başarılı dönüş
                 }

                 /**
                  * @brief Cihaz kapatıldığında çağrılan fonksiyon.
                   * 
                    * Bu fonksiyon kaynakları serbest bırakmaz, yalnızca bir bilgi yazdırır.
                     */
                     static int dev_release(struct inode *inode, struct file *file) {
                         printk(KERN_INFO "%s: Cihaz kapatıldı.\n", DEVICE_NAME);
                             return 0; // Başarılı dönüş
                             }

                             /**
                              * @brief Cihazdan veri okuma işlemi.
                               * 
                                * Kullanıcıdan okuma isteği geldiğinde tampondan veriyi kullanıcı uzayına kopyalar.
                                 * 
                                  * @param file Açık dosya yapısı
                                   * @param user_buffer Kullanıcı uzayındaki hedef tampon
                                    * @param size Okunmak istenen veri boyutu
                                     * @param offset Tampondaki okuma başlangıç konumu
                                      * @return ssize_t Okunan bayt sayısı ya da hata kodu
                                       */
                                       static ssize_t dev_read(struct file *file, char __user *user_buffer, size_t size, loff_t *offset) {
                                           size_t bytes_to_read = BUFFER_SIZE - *offset < size ? BUFFER_SIZE - *offset : size;

                                               if (bytes_to_read == 0) {
                                                       printk(KERN_INFO "%s: Tamponun sonuna ulaşıldı.\n", DEVICE_NAME);
                                                               return 0; // Daha fazla veri yok
                                                                   }

                                                                       if (copy_to_user(user_buffer, device_buffer + *offset, bytes_to_read)) {
                                                                               return -EFAULT; // Hata: Veriyi kullanıcı alanına kopyalayamadı
                                                                                   }

                                                                                       *offset += bytes_to_read;
                                                                                           printk(KERN_INFO "%s: %zu bayt cihazdan okundu.\n", DEVICE_NAME, bytes_to_read);
                                                                                               return bytes_to_read;
                                                                                               }

                                                                                               /**
                                                                                                * @brief Cihaza veri yazma işlemi.
                                                                                                 * 
                                                                                                  * Kullanıcıdan yazma isteği geldiğinde veriyi cihaz tamponuna kopyalar.
                                                                                                   * 
                                                                                                    * @param file Açık dosya yapısı
                                                                                                     * @param user_buffer Kullanıcı uzayındaki kaynak tampon
                                                                                                      * @param size Yazılmak istenen veri boyutu
                                                                                                       * @param offset Tampondaki yazma başlangıç konumu
                                                                                                        * @return ssize_t Yazılan bayt sayısı ya da hata kodu
                                                                                                         */
                                                                                                         static ssize_t dev_write(struct file *file, const char __user *user_buffer, size_t size, loff_t *offset) {
                                                                                                             size_t bytes_to_write = BUFFER_SIZE - *offset < size ? BUFFER_SIZE - *offset : size;

                                                                                                                 if (bytes_to_write == 0) {
                                                                                                                         printk(KERN_INFO "%s: Tampon dolu.\n", DEVICE_NAME);
                                                                                                                                 return -ENOMEM; // Yeterli alan yok
                                                                                                                                     }

                                                                                                                                         if (copy_from_user(device_buffer + *offset, user_buffer, bytes_to_write)) {
                                                                                                                                                 return -EFAULT; // Hata: Veriyi kullanıcı alanından kopyalayamadı
                                                                                                                                                     }

                                                                                                                                                         *offset += bytes_to_write;
                                                                                                                                                             printk(KERN_INFO "%s: %zu bayt cihaza yazıldı.\n", DEVICE_NAME, bytes_to_write);
                                                                                                                                                                 return bytes_to_write;
                                                                                                                                                                 }

                                                                                                                                                                 // Dosya işlemleri yapılandırması
                                                                                                                                                                 static struct file_operations fops = {
                                                                                                                                                                     .owner = THIS_MODULE,
                                                                                                                                                                         .open = dev_open,
                                                                                                                                                                             .release = dev_release,
                                                                                                                                                                                 .read = dev_read,
                                                                                                                                                                                     .write = dev_write,
                                                                                                                                                                                     };

                                                                                                                                                                                     /**
                                                                                                                                                                                      * @brief Sürücü başlatma fonksiyonu.
                                                                                                                                                                                       * 
                                                                                                                                                                                        * Sürücü yüklendiğinde çalışır. Cihaz numarası tahsis eder ve karakter cihazı kaydeder.
                                                                                                                                                                                         * 
                                                                                                                                                                                          * @return int Başarılıysa 0, aksi halde hata kodu
                                                                                                                                                                                           */
                                                                                                                                                                                           static int __init char_driver_init(void) {
                                                                                                                                                                                               // Cihaz numarası tahsis et
                                                                                                                                                                                                   if (alloc_chrdev_region(&dev_number, 0, 1, DEVICE_NAME) < 0) {
                                                                                                                                                                                                           printk(KERN_ERR "%s: Major numarası tahsis edilemedi.\n", DEVICE_NAME);
                                                                                                                                                                                                                   return -1;
                                                                                                                                                                                                                       }

                                                                                                                                                                                                                           // Karakter cihazı başlat ve kaydet
                                                                                                                                                                                                                               cdev_init(&char_device, &fops);
                                                                                                                                                                                                                                   if (cdev_add(&char_device, dev_number, 1) < 0) {
                                                                                                                                                                                                                                           unregister_chrdev_region(dev_number, 1);
                                                                                                                                                                                                                                                   printk(KERN_ERR "%s: Cihaz kaydı başarısız oldu.\n", DEVICE_NAME);
                                                                                                                                                                                                                                                           return -1;
                                                                                                                                                                                                                                                               }

                                                                                                                                                                                                                                                                   printk(KERN_INFO "%s: Major %d, Minor %d ile cihaz kaydedildi.\n", DEVICE_NAME, MAJOR(dev_number), MINOR(dev_number));
                                                                                                                                                                                                                                                                       return 0;
                                                                                                                                                                                                                                                                       }

                                                                                                                                                                                                                                                                       /**
                                                                                                                                                                                                                                                                        * @brief Sürücü temizleme fonksiyonu.
                                                                                                                                                                                                                                                                         * 
                                                                                                                                                                                                                                                                          * Sürücü kaldırıldığında çalışır. Cihazı serbest bırakır ve kaydı siler.
                                                                                                                                                                                                                                                                           */
                                                                                                                                                                                                                                                                           static void __exit char_driver_exit(void) {
                                                                                                                                                                                                                                                                               cdev_del(&char_device);
                                                                                                                                                                                                                                                                                   unregister_chrdev_region(dev_number, 1);
                                                                                                                                                                                                                                                                                       printk(KERN_INFO "%s: Cihaz kaydı silindi.\n", DEVICE_NAME);
                                                                                                                                                                                                                                                                                       }

                                                                                                                                                                                                                                                                                       module_init(char_driver_init);
                                                                                                                                                                                                                                                                                       module_exit(char_driver_exit);

                                                                                                                                                                                                                                                                                       MODULE_LICENSE("GPL"); // Lisans bilgisi
                                                                                                                                                                                                                                                                                       MODULE_AUTHOR("Volkan SARI "); // Yazar bilgisi
                                                                                                                                                                                                                                                                                       MODULE_DESCRIPTION("Basit Linux Karakter Cihaz Sürücüsü"I); // Açıklama