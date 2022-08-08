make clean
make 
make flash 

curl -F fileupload1=@/home/leon/work/gnu/new/GNU_SDK_V15.02/GNU_SDK_V15.01/tools/image_gen_cstm/output/output.img -F press="Upload files" http://192.168.1.192:7000/hfs_test/
