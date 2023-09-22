- apply the patch for your srsran
```
git clone https://github.com/srsRAN/srsRAN.git
git checkout release_21_10
git am <path to srsenb.patch> --whitespace=nowarn
```
- rebuild the srsenb
```
cd <srsran build folder>
make srsenb
```