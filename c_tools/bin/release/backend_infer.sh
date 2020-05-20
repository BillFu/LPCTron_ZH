
mkdir -p output 

chmod a+x backend_infer

./backend_infer 1008 1008_ipa.txt tacotron.pb 16k output 
