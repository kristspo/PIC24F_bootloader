
/* Set configuration bits according to PIC24 device */

#if defined(__PIC24F08KA101__) || defined(__PIC24F08KA102__) || defined(__PIC24F16KA101__) || defined(__PIC24F16KA102__)
    #include "config_bits/p24F16KA10x.h"
#endif
#if defined(__PIC24F16KA301__) || defined(__PIC24F16KA302__) || defined(__PIC24F16KA304__)
    #include "config_bits/p24F16KA30x.h"
#endif
#if defined(__PIC24F32KA301__) || defined(__PIC24F32KA302__) || defined(__PIC24F32KA304__)
    #include "config_bits/p24F32KA30x.h"
#endif
