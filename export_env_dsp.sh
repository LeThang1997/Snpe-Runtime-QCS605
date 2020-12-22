#!/system/bin/sh
#Device-side
export SNPE_ROOT=/data/bkavai/snpe
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$SNPE_ROOT/lib
export PATH=$PATH:/system/bin
export ADSP_LIBRARY_PATH="$SNPE_ROOT/dsp/lib;/system/lib/rfsa/adsp;/system/vendor/lib/rfsa/adsp;/dsp"
echo "Bkav exported environment for DSP successfully"
