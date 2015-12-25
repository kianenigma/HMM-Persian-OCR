 for i in `seq 0 9`;
        do
                HVite -A -T 1 -o ST -p -17 -s 50 -S test_sh_$i.lst -H hmm/hmm_32/Macros_hmm -l "*" -i res$i.mlf -w Network Dictionary HMMsList
		HResults -I samplesRef.mlf HMMsList res$i.mlf >> result_$i.res
        done    
