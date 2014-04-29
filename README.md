SkpToStl
========

convert sketchup (skp format) to stl format by Cpp (skp格式转换到stl格式的cpp实现)


程序
有三个参数
SkpToStl  -i  importFile.skp -e  exportFile.stl -o
-i  必须，要导入的skp文件的文件名，可以绝对路径或者相对路径
-e  必须，要导入的stl文件的文件名 ，可以绝对路径和相对路径
-o  可选的，是否进行自动优化（可能较耗时间）
只能在windows平台下运行，根据计算机选择32位版本或者64位版本。


程序大意
1.
Sketchup文件的所有面都是一个entities类型的集合
每个entities是一个3d模型各个模块的集合
entities集合里面包含着entities，group， componentInstance
获取entitis，递归处理这个集合，将里面的group和ComponentInstance提取并且记录其转置矩阵转换为entities再处理。

2.
直接转置之后，有可能造成三角片面不符合右手法则。可以根据叉积判断一下

3.
-o优化
每个三角面中点按照法向量取反射出，如果存在与某个三角面片相交，那么我们认为。这个面是反的，自动纠正其法向量和三角点的顺序方向。
