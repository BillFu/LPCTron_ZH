
for i in ../dataset/biaobei/pcms/*.s16
do
    cat "$i" >> final.pcm
    echo $i
done
echo "Final.pcm created..."