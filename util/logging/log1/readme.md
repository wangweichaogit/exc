纯c的日志库，无锁单线程操作
日志目录在用户目录下的log文件夹下的mylog.log中
要确保log文件夹存在，否则日志记录会失败

多线程间的日志库，就要考虑原子操作(加锁)

也可以一个独立的线程写日志，其他线程把信息输出到任务队列，日志线程从队列中拿消息写日志
writen by wwc 2016/3/30
