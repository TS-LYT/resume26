# 第 12 阶段：C++ 类、继承、多态

## 学习顺序

```text
classes             类的基本封装：成员变量、成员函数、构造函数
access_const_static 访问权限、const 成员函数、static 成员
object_lifecycle    构造、析构、拷贝构造、赋值运算符
inheritance         继承：基类和派生类
polymorphism        多态：虚函数、override、基类指针
virtual_destructor  虚析构函数和接口类
```

## 面试主线

类的核心是封装；继承用于表达 is-a 关系；多态通过虚函数实现运行时绑定。只要基类可能通过指针删除派生类对象，基类析构函数就应该是 `virtual`。
