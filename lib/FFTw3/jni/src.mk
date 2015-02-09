LOCAL_SRC_FILES := ./dft/plan.c ./dft/zero.c ./dft/kdft-dif.c ./dft/nop.c ./dft/bluestein.c ./dft/indirect-transpose.c ./dft/dftw-direct.c ./dft/problem.c ./dft/rank-geq2.c ./dft/dftw-directsq.c ./dft/dftw-generic.c ./dft/scalar/codelets/t2_16.c ./dft/scalar/codelets/n1_2.c ./dft/scalar/codelets/q1_8.c ./dft/scalar/codelets/t1_9.c ./dft/scalar/codelets/t1_20.c ./dft/scalar/codelets/n1_16.c ./dft/scalar/codelets/t2_10.c ./dft/scalar/codelets/t1_3.c ./dft/scalar/codelets/n1_4.c ./dft/scalar/codelets/n1_6.c ./dft/scalar/codelets/n1_7.c ./dft/scalar/codelets/q1_2.c ./dft/scalar/codelets/q1_3.c ./dft/scalar/codelets/n1_12.c ./dft/scalar/codelets/n1_8.c ./dft/scalar/codelets/q1_6.c ./dft/scalar/codelets/n1_32.c ./dft/scalar/codelets/t1_16.c ./dft/scalar/codelets/t1_7.c ./dft/scalar/codelets/t2_4.c ./dft/scalar/codelets/n1_3.c ./dft/scalar/codelets/t1_4.c ./dft/scalar/codelets/n1_64.c ./dft/scalar/codelets/t1_15.c ./dft/scalar/codelets/t1_6.c ./dft/scalar/codelets/t1_32.c ./dft/scalar/codelets/n1_5.c ./dft/scalar/codelets/t1_64.c ./dft/scalar/codelets/n1_25.c ./dft/scalar/codelets/t1_8.c ./dft/scalar/codelets/t2_20.c ./dft/scalar/codelets/t1_12.c ./dft/scalar/codelets/t1_10.c ./dft/scalar/codelets/q1_4.c ./dft/scalar/codelets/t2_64.c ./dft/scalar/codelets/n1_10.c ./dft/scalar/codelets/n1_15.c ./dft/scalar/codelets/n1_14.c ./dft/scalar/codelets/t2_5.c ./dft/scalar/codelets/t2_25.c ./dft/scalar/codelets/n1_11.c ./dft/scalar/codelets/t1_2.c ./dft/scalar/codelets/t1_25.c ./dft/scalar/codelets/t1_5.c ./dft/scalar/codelets/n1_20.c ./dft/scalar/codelets/n1_13.c ./dft/scalar/codelets/t2_32.c ./dft/scalar/codelets/n1_9.c ./dft/scalar/codelets/codlist.c ./dft/scalar/codelets/q1_5.c ./dft/scalar/codelets/t2_8.c ./dft/scalar/t.c ./dft/scalar/n.c ./dft/rader.c ./dft/ct.c ./dft/kdft.c ./dft/dftw-genericbuf.c ./dft/generic.c ./dft/kdft-difsq.c ./dft/buffered.c ./dft/conf.c ./dft/kdft-dit.c ./dft/vrank-geq1.c ./dft/solve.c ./dft/direct.c ./dft/indirect.c ./kernel/tensor4.c ./kernel/plan.c ./kernel/cpy2d-pair.c ./kernel/tensor8.c ./kernel/tile2d.c ./kernel/stride.c ./kernel/awake.c ./kernel/kalloc.c ./kernel/debug.c ./kernel/ops.c ./kernel/primes.c ./kernel/timer.c ./kernel/tensor3.c ./kernel/problem.c ./kernel/trig.c ./kernel/planner.c ./kernel/extract-reim.c ./kernel/align.c ./kernel/pickdim.c ./kernel/cpy2d.c ./kernel/rader.c ./kernel/ct.c ./kernel/tensor7.c ./kernel/minmax.c ./kernel/transpose.c ./kernel/md5-1.c ./kernel/twiddle.c ./kernel/scan.c ./kernel/cpy1d.c ./kernel/buffered.c ./kernel/print.c ./kernel/tensor5.c ./kernel/tensor1.c ./kernel/tensor2.c ./kernel/iabs.c ./kernel/md5.c ./kernel/tensor9.c ./kernel/solvtab.c ./kernel/alloc.c ./kernel/assert.c ./kernel/tensor.c ./kernel/hash.c ./kernel/solver.c ./rdft/problem2.c ./rdft/plan.c ./rdft/rank0-rdft2.c ./rdft/dht-r2hc.c ./rdft/vrank-geq1-rdft2.c ./rdft/kr2r.c ./rdft/kr2c.c ./rdft/dht-rader.c ./rdft/rdft2-inplace-strides.c ./rdft/nop.c ./rdft/rank0.c ./rdft/solve2.c ./rdft/problem.c ./rdft/ct-hc2c.c ./rdft/rank-geq2.c ./rdft/plan2.c ./rdft/scalar/r2c.c ./rdft/scalar/r2cf/r2cfII_6.c ./rdft/scalar/r2cf/r2cf_8.c ./rdft/scalar/r2cf/r2cf_20.c ./rdft/scalar/r2cf/hf2_4.c ./rdft/scalar/r2cf/hc2cfdft_10.c ./rdft/scalar/r2cf/r2cf_16.c ./rdft/scalar/r2cf/r2cfII_10.c ./rdft/scalar/r2cf/r2cfII_3.c ./rdft/scalar/r2cf/r2cf_2.c ./rdft/scalar/r2cf/hf_3.c ./rdft/scalar/r2cf/hf_5.c ./rdft/scalar/r2cf/hc2cf_10.c ./rdft/scalar/r2cf/r2cf_128.c ./rdft/scalar/r2cf/r2cf_13.c ./rdft/scalar/r2cf/hc2cf2_16.c ./rdft/scalar/r2cf/hc2cf_12.c ./rdft/scalar/r2cf/hc2cf2_8.c ./rdft/scalar/r2cf/hc2cfdft_16.c ./rdft/scalar/r2cf/r2cfII_32.c ./rdft/scalar/r2cf/hf_20.c ./rdft/scalar/r2cf/hf_7.c ./rdft/scalar/r2cf/r2cf_11.c ./rdft/scalar/r2cf/r2cfII_8.c ./rdft/scalar/r2cf/hf_10.c ./rdft/scalar/r2cf/hc2cfdft2_32.c ./rdft/scalar/r2cf/r2cfII_9.c ./rdft/scalar/r2cf/hc2cfdft_12.c ./rdft/scalar/r2cf/r2cf_12.c ./rdft/scalar/r2cf/r2cfII_25.c ./rdft/scalar/r2cf/r2cfII_20.c ./rdft/scalar/r2cf/r2cfII_15.c ./rdft/scalar/r2cf/hf2_5.c ./rdft/scalar/r2cf/hc2cf_16.c ./rdft/scalar/r2cf/hc2cfdft_32.c ./rdft/scalar/r2cf/hf_16.c ./rdft/scalar/r2cf/r2cf_64.c ./rdft/scalar/r2cf/r2cf_25.c ./rdft/scalar/r2cf/r2cf_6.c ./rdft/scalar/r2cf/hf2_20.c ./rdft/scalar/r2cf/hc2cfdft2_4.c ./rdft/scalar/r2cf/hf_64.c ./rdft/scalar/r2cf/hf2_16.c ./rdft/scalar/r2cf/r2cfII_4.c ./rdft/scalar/r2cf/r2cf_3.c ./rdft/scalar/r2cf/hc2cf2_20.c ./rdft/scalar/r2cf/hf_32.c ./rdft/scalar/r2cf/r2cf_14.c ./rdft/scalar/r2cf/r2cfII_12.c ./rdft/scalar/r2cf/r2cf_10.c ./rdft/scalar/r2cf/hc2cf_4.c ./rdft/scalar/r2cf/r2cf_32.c ./rdft/scalar/r2cf/hf_25.c ./rdft/scalar/r2cf/r2cf_15.c ./rdft/scalar/r2cf/r2cfII_5.c ./rdft/scalar/r2cf/r2cfII_7.c ./rdft/scalar/r2cf/hc2cfdft_8.c ./rdft/scalar/r2cf/hc2cf_20.c ./rdft/scalar/r2cf/hc2cf_2.c ./rdft/scalar/r2cf/hc2cfdft2_8.c ./rdft/scalar/r2cf/hc2cfdft_6.c ./rdft/scalar/r2cf/r2cf_4.c ./rdft/scalar/r2cf/hf_8.c ./rdft/scalar/r2cf/hc2cf_6.c ./rdft/scalar/r2cf/hf_4.c ./rdft/scalar/r2cf/hc2cf2_32.c ./rdft/scalar/r2cf/hc2cfdft2_20.c ./rdft/scalar/r2cf/hc2cfdft_2.c ./rdft/scalar/r2cf/r2cfII_2.c ./rdft/scalar/r2cf/hf2_8.c ./rdft/scalar/r2cf/hc2cfdft_4.c ./rdft/scalar/r2cf/r2cfII_16.c ./rdft/scalar/r2cf/hf_12.c ./rdft/scalar/r2cf/r2cfII_64.c ./rdft/scalar/r2cf/r2cf_7.c ./rdft/scalar/r2cf/hf2_25.c ./rdft/scalar/r2cf/hc2cf2_4.c ./rdft/scalar/r2cf/hf_9.c ./rdft/scalar/r2cf/r2cf_9.c ./rdft/scalar/r2cf/hf_15.c ./rdft/scalar/r2cf/hf_6.c ./rdft/scalar/r2cf/hc2cf_8.c ./rdft/scalar/r2cf/r2cf_5.c ./rdft/scalar/r2cf/hc2cf_32.c ./rdft/scalar/r2cf/hc2cfdft_20.c ./rdft/scalar/r2cf/hf_2.c ./rdft/scalar/r2cf/hc2cfdft2_16.c ./rdft/scalar/r2cf/codlist.c ./rdft/scalar/r2cf/hf2_32.c ./rdft/scalar/r2cb/hc2cbdft_10.c ./rdft/scalar/r2cb/hb_64.c ./rdft/scalar/r2cb/hb_32.c ./rdft/scalar/r2cb/r2cbIII_5.c ./rdft/scalar/r2cb/r2cbIII_6.c ./rdft/scalar/r2cb/hb_9.c ./rdft/scalar/r2cb/r2cb_12.c ./rdft/scalar/r2cb/hb2_32.c ./rdft/scalar/r2cb/hb_2.c ./rdft/scalar/r2cb/hc2cbdft_16.c ./rdft/scalar/r2cb/r2cb_3.c ./rdft/scalar/r2cb/hb2_25.c ./rdft/scalar/r2cb/hc2cb_12.c ./rdft/scalar/r2cb/hc2cb_6.c ./rdft/scalar/r2cb/r2cbIII_15.c ./rdft/scalar/r2cb/hc2cb2_20.c ./rdft/scalar/r2cb/hb_15.c ./rdft/scalar/r2cb/hb_5.c ./rdft/scalar/r2cb/hb2_5.c ./rdft/scalar/r2cb/r2cb_2.c ./rdft/scalar/r2cb/hc2cb_32.c ./rdft/scalar/r2cb/r2cb_14.c ./rdft/scalar/r2cb/hc2cb2_8.c ./rdft/scalar/r2cb/hc2cbdft_8.c ./rdft/scalar/r2cb/r2cb_25.c ./rdft/scalar/r2cb/hc2cbdft_6.c ./rdft/scalar/r2cb/r2cbIII_16.c ./rdft/scalar/r2cb/hc2cbdft2_4.c ./rdft/scalar/r2cb/hc2cb_10.c ./rdft/scalar/r2cb/hc2cbdft2_32.c ./rdft/scalar/r2cb/r2cb_13.c ./rdft/scalar/r2cb/hc2cbdft2_20.c ./rdft/scalar/r2cb/hc2cb_20.c ./rdft/scalar/r2cb/hc2cb_8.c ./rdft/scalar/r2cb/r2cbIII_4.c ./rdft/scalar/r2cb/r2cbIII_2.c ./rdft/scalar/r2cb/hb_12.c ./rdft/scalar/r2cb/hc2cbdft_12.c ./rdft/scalar/r2cb/hb2_20.c ./rdft/scalar/r2cb/r2cb_64.c ./rdft/scalar/r2cb/hb_8.c ./rdft/scalar/r2cb/hb_6.c ./rdft/scalar/r2cb/r2cbIII_3.c ./rdft/scalar/r2cb/r2cb_4.c ./rdft/scalar/r2cb/r2cb_32.c ./rdft/scalar/r2cb/hb_4.c ./rdft/scalar/r2cb/r2cbIII_64.c ./rdft/scalar/r2cb/hb_3.c ./rdft/scalar/r2cb/hb_25.c ./rdft/scalar/r2cb/hb_20.c ./rdft/scalar/r2cb/hc2cbdft_4.c ./rdft/scalar/r2cb/hc2cb_4.c ./rdft/scalar/r2cb/hb2_16.c ./rdft/scalar/r2cb/r2cb_8.c ./rdft/scalar/r2cb/hb2_8.c ./rdft/scalar/r2cb/hc2cb2_16.c ./rdft/scalar/r2cb/hc2cbdft2_8.c ./rdft/scalar/r2cb/hc2cbdft2_16.c ./rdft/scalar/r2cb/r2cbIII_8.c ./rdft/scalar/r2cb/hc2cb2_4.c ./rdft/scalar/r2cb/hc2cbdft_20.c ./rdft/scalar/r2cb/r2cbIII_20.c ./rdft/scalar/r2cb/r2cb_128.c ./rdft/scalar/r2cb/r2cbIII_25.c ./rdft/scalar/r2cb/r2cbIII_32.c ./rdft/scalar/r2cb/r2cb_11.c ./rdft/scalar/r2cb/hc2cb_16.c ./rdft/scalar/r2cb/r2cb_6.c ./rdft/scalar/r2cb/r2cb_7.c ./rdft/scalar/r2cb/r2cb_9.c ./rdft/scalar/r2cb/hb_7.c ./rdft/scalar/r2cb/hc2cbdft_32.c ./rdft/scalar/r2cb/hb_16.c ./rdft/scalar/r2cb/r2cbIII_9.c ./rdft/scalar/r2cb/hc2cb_2.c ./rdft/scalar/r2cb/r2cbIII_10.c ./rdft/scalar/r2cb/r2cb_16.c ./rdft/scalar/r2cb/r2cb_10.c ./rdft/scalar/r2cb/r2cb_5.c ./rdft/scalar/r2cb/hc2cbdft_2.c ./rdft/scalar/r2cb/hb_10.c ./rdft/scalar/r2cb/r2cbIII_7.c ./rdft/scalar/r2cb/r2cbIII_12.c ./rdft/scalar/r2cb/r2cb_20.c ./rdft/scalar/r2cb/hb2_4.c ./rdft/scalar/r2cb/r2cb_15.c ./rdft/scalar/r2cb/hc2cb2_32.c ./rdft/scalar/r2cb/codlist.c ./rdft/scalar/hfb.c ./rdft/scalar/r2r.c ./rdft/scalar/r2r/e10_8.c ./rdft/scalar/r2r/e01_8.c ./rdft/scalar/r2r/codlist.c ./rdft/scalar/hc2c.c ./rdft/vrank3-transpose.c ./rdft/direct-r2c.c ./rdft/direct2.c ./rdft/nop2.c ./rdft/khc2hc.c ./rdft/hc2hc-direct.c ./rdft/rdft-dht.c ./rdft/generic.c ./rdft/hc2hc.c ./rdft/rank-geq2-rdft2.c ./rdft/direct-r2r.c ./rdft/khc2c.c ./rdft/buffered.c ./rdft/rdft2-tensor-max-index.c ./rdft/rdft2-strides.c ./rdft/conf.c ./rdft/vrank-geq1.c ./rdft/hc2hc-generic.c ./rdft/solve.c ./rdft/buffered2.c ./rdft/indirect.c ./rdft/rdft2-rdft.c ./rdft/dft-r2hc.c ./rdft/ct-hc2c-direct.c ./reodft/reodft11e-r2hc.c ./reodft/reodft010e-r2hc.c ./reodft/reodft00e-splitradix.c ./reodft/rodft00e-r2hc.c ./reodft/conf.c ./reodft/redft00e-r2hc-pad.c ./reodft/rodft00e-r2hc-pad.c ./reodft/reodft11e-r2hc-odd.c ./reodft/reodft11e-radix2.c ./reodft/redft00e-r2hc.c ./api/plan-dft-c2r-3d.c ./api/apiplan.c ./api/plan-guru64-split-dft-c2r.c ./api/import-wisdom.c ./api/plan-dft-c2r.c ./api/plan-dft-3d.c ./api/version.c ./api/mkprinter-file.c ./api/plan-guru64-split-dft-r2c.c ./api/plan-r2r-1d.c ./api/plan-dft.c ./api/malloc.c ./api/plan-guru64-r2r.c ./api/import-system-wisdom.c ./api/plan-dft-c2r-2d.c ./api/import-wisdom-from-file.c ./api/execute-dft-c2r.c ./api/execute-split-dft-c2r.c ./api/plan-guru-split-dft-c2r.c ./api/print-plan.c ./api/forget-wisdom.c ./api/plan-guru-dft.c ./api/plan-dft-r2c.c ./api/execute-split-dft.c ./api/plan-dft-r2c-1d.c ./api/plan-guru-r2r.c ./api/import-wisdom-from-string.c ./api/export-wisdom.c ./api/plan-guru-split-dft.c ./api/mktensor-iodims64.c ./api/plan-dft-r2c-3d.c ./api/execute-split-dft-r2c.c ./api/mktensor-rowmajor.c ./api/execute.c ./api/mapflags.c ./api/plan-guru-dft-r2c.c ./api/plan-r2r.c ./api/plan-dft-2d.c ./api/f77api.c ./api/mkprinter-str.c ./api/plan-many-dft-r2c.c ./api/the-planner.c ./api/execute-dft-r2c.c ./api/plan-r2r-2d.c ./api/mktensor-iodims.c ./api/plan-many-dft-c2r.c ./api/plan-r2r-3d.c ./api/execute-dft.c ./api/plan-guru64-dft-r2c.c ./api/export-wisdom-to-file.c ./api/plan-guru64-split-dft.c ./api/plan-guru64-dft-c2r.c ./api/plan-dft-1d.c ./api/plan-many-r2r.c ./api/plan-guru64-dft.c ./api/plan-guru-split-dft-r2c.c ./api/export-wisdom-to-string.c ./api/plan-dft-c2r-1d.c ./api/rdft2-pad.c ./api/plan-guru-dft-c2r.c ./api/plan-dft-r2c-2d.c ./api/flops.c ./api/plan-many-dft.c ./api/configure.c ./api/map-r2r-kind.c ./api/execute-r2r.c
