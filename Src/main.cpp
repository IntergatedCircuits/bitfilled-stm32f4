#include "bitfilled.hpp"
#include "stm32f4xx_ll_gpio.h"

struct gpio
{
#if USE_BIT_BAND
    using mmr_ops = bitfilled::bitband<PERIPH_BASE>;
#else
    using mmr_ops = bitfilled::base;
#endif
    enum class mode
    {
        input = 0,
        output = 1,
        alternate = 2,
        analog = 3
    };
    struct moder : BF_MMREG(std::uint32_t, rw, mmr_ops)
    {
        BF_COPY_SUPERCLASS(moder)
        BF_MMREGBITSET(mode, rw, 2, 16, 0) MODE;
    } MODER;
    struct otyper : BF_MMREG(std::uint32_t, rw, mmr_ops)
    {
        BF_COPY_SUPERCLASS(otyper)
        BF_MMREGBITSET(std::uint32_t, rw, 1, 16, 0) OTYPE;
    } OTYPER;
    struct ospeedr : BF_MMREG(std::uint32_t, rw, mmr_ops)
    {
        BF_COPY_SUPERCLASS(ospeedr)
        BF_MMREGBITSET(std::uint32_t, rw, 2, 16, 0) OSPEED;
    } OSPEEDR;
    struct pupdr : BF_MMREG(std::uint32_t, rw, mmr_ops)
    {
        BF_COPY_SUPERCLASS(pupdr)
        BF_MMREGBITSET(std::uint32_t, rw, 2, 16, 0) PUPD;
    } PUPDR;
    struct idr : BF_MMREG(std::uint32_t, r, mmr_ops)
    {
        BF_COPY_SUPERCLASS(idr)
        BF_MMREGBITSET(std::uint32_t, r, 1, 16, 0) ID;
    } IDR;
    struct odr : BF_MMREG(std::uint32_t, rw, mmr_ops)
    {
        BF_COPY_SUPERCLASS(odr)
        BF_MMREGBITSET(std::uint32_t, rw, 1, 16, 0) OD;
    } ODR;
    struct bsrr : BF_MMREG(std::uint32_t, w, mmr_ops)
    {
        BF_COPY_SUPERCLASS(bsrr)
        BF_MMREGBITSET(std::uint32_t, rw, 1, 16, 0) BS;
        BF_MMREGBITSET(std::uint32_t, rw, 1, 16, 16) BR;
    } BSRR;
    struct lckr : BF_MMREG(std::uint32_t, rw, mmr_ops)
    {
        BF_COPY_SUPERCLASS(lckr)
        BF_MMREGBITSET(std::uint32_t, rw, 1, 16, 0) LCK;
        BF_MMREGBITS(std::uint32_t, rw, 16, 16) LCKK;
    } LCKR;
    struct afr : BF_MMREG(std::uint32_t, rw, mmr_ops)
    {
        BF_COPY_SUPERCLASS(afr)
        BF_MMREGBITSET(std::uint32_t, rw, 4, 8, 0) AFSEL;
    } AFR[2];
};

int main()
{
    RCC->AHB1ENR = RCC->AHB1ENR | RCC_AHB1ENR_GPIOAEN | RCC_AHB1ENR_GPIODEN;
    __DSB();

#if BITFILLED_SOLUTION
    auto& GPIO_A = *reinterpret_cast<volatile gpio*>(GPIOA_BASE);
    auto& GPIO_D = *reinterpret_cast<volatile gpio*>(GPIOD_BASE);

    GPIO_A.MODER.MODE[0] = gpio::mode::input;
    GPIO_D.MODER.MODE[12] = gpio::mode::output;

    while (true)
    {
        // only use read-modify-write ODR register if it's done atomically via bitband CPU operation
        if constexpr (std::is_same_v<gpio::mmr_ops, bitfilled::bitband<PERIPH_BASE>>)
        {
            GPIO_D.ODR.OD[12] = GPIO_A.IDR.ID[0];
        }
        else
        {
            if (GPIO_A.IDR.ID[0] == 0)
                GPIO_D.BSRR.BR[12] = 1;
            else
                GPIO_D.BSRR.BS[12] = 1;
        }
    }
#else
    LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_0, LL_GPIO_MODE_INPUT);
    LL_GPIO_SetPinMode(GPIOD, LL_GPIO_PIN_12, LL_GPIO_MODE_OUTPUT);
    while (true)
    {
        if (LL_GPIO_IsInputPinSet(GPIOA, LL_GPIO_PIN_0))
            LL_GPIO_SetOutputPin(GPIOD, LL_GPIO_PIN_12);
        else
            LL_GPIO_ResetOutputPin(GPIOD, LL_GPIO_PIN_12);
    }
#endif
}
