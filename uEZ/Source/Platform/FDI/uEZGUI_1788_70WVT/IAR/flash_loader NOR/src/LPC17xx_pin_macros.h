#define pdir_eval(n)    n
#define pdir(name, pin)      (pdir_eval(PINDIR_##name##_##pin) << ((pin&31)*1UL))
#define PINCFG_PINDIR(port)      \
            (pdir(port, 0) | \
                pdir(port, 1) | \
                pdir(port, 2) | \
                pdir(port, 3) | \
                pdir(port, 4) | \
                pdir(port, 5) | \
                pdir(port, 6) | \
                pdir(port, 7) | \
                pdir(port, 8) | \
                pdir(port, 9) | \
                pdir(port, 10) | \
                pdir(port, 11) | \
                pdir(port, 12) | \
                pdir(port, 13) | \
                pdir(port, 14) | \
                pdir(port, 15) | \
                pdir(port, 16) | \
                pdir(port, 17) | \
                pdir(port, 18) | \
                pdir(port, 19) | \
                pdir(port, 20) | \
                pdir(port, 21) | \
                pdir(port, 22) | \
                pdir(port, 23) | \
                pdir(port, 24) | \
                pdir(port, 25) | \
                pdir(port, 26) | \
                pdir(port, 27) | \
                pdir(port, 28) | \
                pdir(port, 29) | \
                pdir(port, 30) | \
                pdir(port, 31))

#define pset_eval(n)    n
#define pset(name, pin)      (pset_eval(PINSET_##name##_##pin) << ((pin&31)*1UL))
#define PINCFG_PINSET(port)      \
            (pset(port, 0) | \
                pset(port, 1) | \
                pset(port, 2) | \
                pset(port, 3) | \
                pset(port, 4) | \
                pset(port, 5) | \
                pset(port, 6) | \
                pset(port, 7) | \
                pset(port, 8) | \
                pset(port, 9) | \
                pset(port, 10) | \
                pset(port, 11) | \
                pset(port, 12) | \
                pset(port, 13) | \
                pset(port, 14) | \
                pset(port, 15) | \
                pset(port, 16) | \
                pset(port, 17) | \
                pset(port, 18) | \
                pset(port, 19) | \
                pset(port, 20) | \
                pset(port, 21) | \
                pset(port, 22) | \
                pset(port, 23) | \
                pset(port, 24) | \
                pset(port, 25) | \
                pset(port, 26) | \
                pset(port, 27) | \
                pset(port, 28) | \
                pset(port, 29) | \
                pset(port, 30) | \
                pset(port, 31))

#define pclear_eval(n)    n
#define pclear(name, pin)      (pclear_eval(PINCLR_##name##_##pin) << ((pin&31)*1UL))
#define PINCFG_PINCLR(port)      \
            (pclear(port, 0) | \
                pclear(port, 1) | \
                pclear(port, 2) | \
                pclear(port, 3) | \
                pclear(port, 4) | \
                pclear(port, 5) | \
                pclear(port, 6) | \
                pclear(port, 7) | \
                pclear(port, 8) | \
                pclear(port, 9) | \
                pclear(port, 10) | \
                pclear(port, 11) | \
                pclear(port, 12) | \
                pclear(port, 13) | \
                pclear(port, 14) | \
                pclear(port, 15) | \
                pclear(port, 16) | \
                pclear(port, 17) | \
                pclear(port, 18) | \
                pclear(port, 19) | \
                pclear(port, 20) | \
                pclear(port, 21) | \
                pclear(port, 22) | \
                pclear(port, 23) | \
                pclear(port, 24) | \
                pclear(port, 25) | \
                pclear(port, 26) | \
                pclear(port, 27) | \
                pclear(port, 28) | \
                pclear(port, 29) | \
                pclear(port, 30) | \
                pclear(port, 31))
