# Bitfilled MM I/O demonstration

This simple project serves as a demonstration of how the [bitfilled][bitfilled] C++20 library
provides a better abstraction to memory mapped register operations, than traditional C-based solutions,
in terms of readability, correctness and performance.

## Contents

The project is a **very** stripped down firmware for the STM32F4Discovery board,
the only thing it does is initialize the button and LED GPIOs,
and mirrors the button state on the LED in the main loop.
Please take a look at [`Src/main.cpp`](./Src/main.cpp) to get an idea, and make your own conclusions about readability.
There are 3 build configurations that achieve the same result with different approaches,
all of them are built and tested with [renode][renode] hardware simulation.

### stm32cube

This preset uses the STM32 LL GPIO driver,
a thin wrapper layer over [CMSIS][cmsis] register operations.
It uses **144 bytes** of code memory.
As the LL (just like HAL) driver operates with pin masks instead of pin positions,
the `LL_GPIO_SetPinMode()` call has to recalculate the pin position,
causing the inflated code size compared to the presented alternatives below.
(It would make for an interesting benchmark to see how many pin writes or reads
could offset this extra cost.)

### bitfilled

This preset uses the bitfilled MM register definition found above `main()`,
functionally performing the same operations as the *stm32cube* variant,
however requiring only **80 bytes** of code memory.

### bitfilled-bitband

This preset also uses the bitfilled MM register definition, specialized with bit-band operation support.

<details>
  <summary>Bit-band access primer</summary>

Bit-band access is a relatively obscure feature of ARM Cortex M3 and M4 CPUs,
where the first 1MB address space of SRAM and peripheral regions have their respective 32MB bit-band
alias address spaces, which allow accessing an individual bit atomically,
with the CPU performing the read-modify-write cycle internally.
A great feature on paper, it never saw widespread adoption,
and later MCUs of the series dropped it entirely.
IMHO its failure can be largely attributed to a lack of convenient software abstraction method,
including a complete exclusion from the scope of [CMSIS][cmsis], ARM's own standard.
It's also difficult to adopt on e.g. the STM32 platform, as their different MCU series map the peripherals in wildly
different addresses, so any portable implementation must be able to easily switch between
bit-band and regular code paths.

</details>

---

Following the same code path as before, the bit-band support increases code size to **100 bytes**,
which can be attributed to the cost of bit address mapping calculations.
However, with the assurance of atomic read-modify-write bit access,
it is possible to use a more optimal solution (copying the input bit to the output bit location).
This brings the code size down to **84 bytes** - still worse than the base bitfilled approach. Another problem is that the renode simulation of this build fails (the code itself works as expected, [bug report][renode-bitband-issue]).

The bitfilled library manages to implement bit-band support without any change in the API,
at the cost of increased code size.
With that in mind, the current conclusion on bit-band use is this:
use them where concurrent access poses a threat to data integrity
(so on peripherals with shared use, e.g. RCC, GPIO).
Note that bit-band support can be selected on the individual register level
(and there's no technical limitation against enabling it on individual register bit level).


[bitfilled]: https://github.com/IntergatedCircuits/bitfilled
[cmsis]: https://arm-software.github.io/CMSIS_6/latest/General/index.html
[renode]: https://renode.io/about/
[renode-bitband-issue]: https://github.com/renode/renode/issues/829
