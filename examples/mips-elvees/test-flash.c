#include <runtime/lib.h>
#include <kernel/uos.h>
#include <elvees/spi.h>
#include <timer/timer.h>

#ifdef TST4153
#define M25PXX
//#define S25FL
#else
//#define M25PXX
//#define AT45DBXX
//#define SDHC_SPI
#define S25FL
#endif

#define SPI_NUM     0
#define SPI_CS_NUM  0

#if defined(M25PXX)
    #include <flash/m25pxx.h>
    const char *flash_name = "M25Pxx";
    typedef m25pxx_t flash_t;
#elif defined(AT45DBXX)
    #include <flash/at45dbxx.h>
    const char *flash_name = "AT45DBxx";
    typedef at45dbxx_t flash_t;
#elif defined(SDHC_SPI)
    #include <flash/sdhc-spi.h>
    const char *flash_name = "SD over SPI";
    typedef sdhc_spi_t flash_t;
#elif defined(S25FL)
    #include <flash/s25fl.h>
    const char *flash_name = "S25FL";
    typedef s25fl_t flash_t;
#endif

#define SPI_FREQUENCY   10000000ul

#define ERASE_SECTOR    5

ARRAY (task, 2000);
elvees_spim_t spi;
flash_t flash;
timer_t timer;

uint32_t buf[1024] __attribute__((aligned(8)));

void hello (void *arg)
{
    flashif_t *f = (flashif_t *)arg;
    unsigned i, j;
    int cnt = 0;
    unsigned long t0, t1;
    
    debug_printf("Checking SD\n");

    while (flash_connect(f) != FLASH_ERR_OK){
        debug_printf("%s not found, ack %*#D\n", flash_name
                , sizeof(flash.databuf), flash.databuf
                );
    }

    debug_printf("Found %s, size: %u Kb, nb pages: %u, page size: %d b\n\
        nb sectors: %u, sector size: %d b\n"
            , flash_name
            , (unsigned)(flash_size(f) >> 10)
            , flash_nb_pages(f)
            , flash_page_size(f)
            , flash_nb_sectors(f)
            , flash_sector_size(f)
            );
    
    debug_printf("Erasing all... ");
    t0 = timer_milliseconds(&timer);
    if (flash_erase_all(f) == FLASH_ERR_OK) {
        t1 = timer_milliseconds(&timer);
        debug_printf("OK! took %d ms, rate: %d bytes/sec\n", 
            t1 - t0, 1000 * (int)(flash_size(f) / (t1 - t0)));
    }
    else debug_printf("FAIL!\n");
    
    // DEBUG
    f->nb_sectors = 16;

    debug_printf("Checking erasure... 00%%");
    t0 = timer_milliseconds(&timer);
    for (i = 0; i < flash_nb_pages(f); ++i) {
        memset(buf, 0, flash_page_size(f));
        if (flash_read(f, i, buf, flash_page_size(f)) != FLASH_ERR_OK) {
            debug_printf("READ FAILED!\n");
            for (;;);
        }
        for (j = 0; j < flash_page_size(f) / 4; ++j)
            if (buf[j] != 0xFFFFFFFF && buf[j] != 0x00000000) {
                debug_printf("FAIL, page #%d, word#%d, data = %08X\n", 
                    i, j, buf[j]);
                for (;;);
            }
        if (i % 1000 == 0)
            debug_printf("\b\b\b%02d%%", i * 100 / flash_nb_pages(f));
    }
    t1 = timer_milliseconds(&timer);
    debug_printf("\b\b\bOK! took %d ms, rate: %d bytes/sec\n", 
            t1 - t0, 1000 * (int)(flash_size(f) / (t1 - t0)));
            
    debug_printf("Filling memory(%dpages * %d) with counter... 00%%"
                , flash_nb_pages(f), flash_page_size(f));
    t0 = timer_milliseconds(&timer);
    i = 0;
    for (i = 0; i < flash_nb_pages(f); ++i) {
        for (j = 0; j < flash_page_size(f) / 4; ++j)
            buf[j] = cnt++;

        if (flash_write(f, i, buf, flash_page_size(f)) != FLASH_ERR_OK) {
            debug_printf("FAIL!\n");
            for (;;);
        }
        if (i % 1000 == 0)
            debug_printf("\b\b\b%02d%%", i * 100 / flash_nb_pages(f));
    }
    t1 = timer_milliseconds(&timer);
    debug_printf("\b\b\bOK! took %d ms, rate: %d bytes/sec\n", 
            t1 - t0, 1000 * (int)(flash_size(f) / (t1 - t0)));

    debug_printf("Checking filling... 00%%");
    t0 = timer_milliseconds(&timer);
    cnt = 0;
    for (i = 0; i < flash_nb_pages(f); ++i) {
        memset(buf, 0, flash_page_size(f));
        if (flash_read(f, i, buf, flash_page_size(f)) != FLASH_ERR_OK) {
            debug_printf("READ FAILED!\n");
            for (;;);
        }
        for (j = 0; j < flash_page_size(f) / 4; ++j)
            if (buf[j] != cnt++) {
                debug_printf("FAIL, page #%x, word%d, data = %08X\n",
                    i, j, buf[j]);
                debug_printf("dump:%256D\n", buf);
                for (;;);
            }
        if (i % 1000 == 0)
            debug_printf("\b\b\b%02d%%", i * 100 / flash_nb_pages(f));
    }
    t1 = timer_milliseconds(&timer);
    debug_printf("\b\b\bOK! took %d ms, rate: %d bytes/sec\n", 
            t1 - t0, 1000 * (int)(flash_size(f) / (t1 - t0)));

    debug_printf("Erasing sector #%d... ", ERASE_SECTOR);
    t0 = timer_milliseconds(&timer);
    if (flash_erase_sectors(f, ERASE_SECTOR, 1) == FLASH_ERR_OK) {
        t1 = timer_milliseconds(&timer);
        debug_printf("OK! took %d ms, rate: %d bytes/sec\n", 
            t1 - t0, 1000 * (int)(flash_sector_size(f) / (t1 - t0)));
    }
    else debug_printf("FAIL!\n");

    debug_printf("Checking erasure... 00%%");
    t0 = timer_milliseconds(&timer);
    cnt = 0;
    for (i = 0; i < flash_nb_pages(f); ++i) {
        memset(buf, 0, flash_page_size(f));
        if (flash_read(f, i, buf, flash_page_size(f)) != FLASH_ERR_OK) {
            debug_printf("READ FAILED!\n");
            for (;;);
        }
        if (i >= ERASE_SECTOR * flash_nb_pages_in_sector(f) &&
            i < (ERASE_SECTOR + 1) * flash_nb_pages_in_sector(f))
        {
            for (j = 0; j < flash_page_size(f) / 4; ++j) {
                if (buf[j] != 0xFFFFFFFF && buf[j] != 0x00000000) {
                    debug_printf("FAIL, page #%x, word%d, data = %08X\n",
                        i, j, buf[j]);
                    //for (;;);
                    break;
                }
            }
        } else {
            cnt = (flash_page_size(f)/4)*i;
            for (j = 0; j < flash_page_size(f) / 4; ++j, ++cnt)
                if (buf[j] != cnt) {
                    debug_printf("FAIL, page #%x, word%d, data = %08X, expect %08X\n",
                        i, j, buf[j], cnt);
                    //for (;;);
                    break;
                }
        }
        if (i % 1000 == 0)
            debug_printf("\b\b\b%02d%%", i * 100 / flash_nb_pages(f));
    }
    t1 = timer_milliseconds(&timer);
    debug_printf("\b\b\bOK! took %d ms, rate: %d bytes/sec\n", 
            t1 - t0, 1000 * (int)(flash_size(f) / (t1 - t0)));

    debug_printf("TEST FINISHED!\n\n");

    for (;;);
}

void uos_init (void)
{
	debug_printf("\n\nTesting %s\n", flash_name);
	
	MC_CLKEN	= 0xffffffff;
    
    timer_init(&timer, KHZ, 1);

    spim_init(&spi, SPI_NUM, SPI_MOSI_OUT | SPI_SS0_OUT | SPI_TSCK_OUT); // | SPI_SS1_OUT
#if defined(M25PXX)
    m25pxx_init(&flash, (spimif_t *)&spi, SPI_FREQUENCY, SPI_MODE_CS_NUM(SPI_CS_NUM));
#elif defined(AT45DBXX)
    at45dbxx_init(&flash, (spimif_t *)&spi, SPI_FREQUENCY, SPI_MODE_CS_NUM(SPI_CS_NUM));
#elif defined(SDHC_SPI)
    sd_spi_init(&flash, (spimif_t *)&spi, SPI_MODE_CS_NUM(SPI_CS_NUM));
#elif defined(S25FL)
    s25fl_init(&flash, (spimif_t *)&spi, SPI_FREQUENCY, SPI_MODE_CS_NUM(SPI_CS_NUM));
#endif
	
	task_create (hello, &flash, "hello", 1, task, sizeof (task));
}
