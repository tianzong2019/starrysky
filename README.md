# starrysky
demo code

- 01_demo_misc
  - misc设备树的demo code
  
- platform-1-byname
  - platform有3种匹配方式，匹配优先级：of_match_table > id_table > name
  - 这里使用name
  
- platform_2_pthread
  - 通过pthread验证current->pid, current->comm 的准确性
  
- platform_3_idtable
  - platform有3种匹配方式，匹配优先级：of_match_table > id_table > name
  - 这里使用 id_table
  
- platform_4_of_match_table
  - platform有3种匹配方式，匹配优先级：of_match_table > id_table > name
  - 这里使用 of_match_table
