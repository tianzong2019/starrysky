# starrysky
demo code

- [misc_1_demo](https://github.com/tianzong2019/starrysky/tree/main/misc_1_demo)
  - misc设备树的demo code
  
- [platform-1-byname](https://github.com/tianzong2019/starrysky/tree/main/platform-1-byname)
  - platform有3种匹配方式，匹配优先级：of_match_table > id_table > name
  - 这里使用name
  
- [platform_2_pthread](https://github.com/tianzong2019/starrysky/tree/main/platform_2_pthread)
  - 通过pthread验证current->pid, current->comm 的准确性
  
- [platform_3_idtable](https://github.com/tianzong2019/starrysky/tree/main/platform_3_idtable)
  - platform有3种匹配方式，匹配优先级：of_match_table > id_table > name
  - 这里使用 id_table
  
- [platform_4_of_match_table](https://github.com/tianzong2019/starrysky/tree/main/platform_4_of_match_table)
  - platform有3种匹配方式，匹配优先级：of_match_table > id_table > name
  - 这里使用 of_match_table

- [platform-5-misc](https://github.com/tianzong2019/starrysky/tree/main/platform-5-misc)
  - 使用module_platform_driver + misc注册字符设备

- [hrtimer-1-demo](https://github.com/tianzong2019/starrysky/tree/main/hrtimer-1-demo)
  - 内核高精度定时器实现

- [hrtimer-2-moduleparam](https://github.com/tianzong2019/starrysky/tree/main/hrtimer-2-moduleparam)
  - 基于内核高精度定时器定时打印module parameter

- [interrupt-1-fasync](https://github.com/tianzong2019/starrysky/tree/main/interrupt-1-fasync)
  - 使用fasync通知user，有中断
  - 采用hrtimer 模拟中断处理

- [interrupt-2-poll](https://github.com/tianzong2019/starrysky/tree/main/interrupt-2-poll)
  - user使用poll轮询中断
  - 采用hrtimer 模拟中断处理

- [interrupt-3-epoll](https://github.com/tianzong2019/starrysky/tree/main/interrupt-3-epoll)
  - user使用 epoll 轮询中断
  - 采用hrtimer 模拟中断处理

- [miniature_driver](https://github.com/tianzong2019/starrysky/tree/main/miniature_driver)
  - 迷你型 misc driver demo code

- [msgqueue](https://github.com/tianzong2019/starrysky/tree/main/msgqueue)
  - 多线程间使用消息队列通信



