-- 单行注释
--[[
    多行注释
    可以写多行内容
]]

-- ============ 变量与类型 ============
local name = "Lua"          -- 字符串
local version = 5.4         -- 数字
local isFast = true         -- 布尔
local nothing = nil         -- 空值

print(type(name))           -- 输出: string

-- ============ 字符串操作 ============
local greeting = "Hello, " .. name .. "!"
print(greeting)
print(#greeting)            -- 长度: 12

-- ============ 表（table）============
-- 表是 Lua 唯一的数据结构，可作数组或字典
local fruits = { "apple", "banana", "orange" }
print(fruits[1])            -- 数组从 1 开始: apple
print(#fruits)              -- 3

local person = {
    name = "张三",
    age = 25,
    hobbies = { "读书", "编程" }
}
print(person.name)          -- 张三
print(person["age"])        -- 25

-- ============ 条件判断 ============
local score = 85
if score >= 90 then
    print("优秀")
elseif score >= 60 then
    print("及格")
else
    print("不及格")
end

-- ============ 循环 ============
-- 数值 for
for i = 1, 5 do
    io.write(i, " ")
end
print()                     -- 1 2 3 4 5

-- 泛型 for
for index, fruit in ipairs(fruits) do
    print(index, fruit)
end

-- while
local count = 0
while count < 3 do
    count = count + 1
end

-- ============ 函数 ============
local function add(a, b)
    return a + b
end
print(add(3, 4))            -- 7

-- 多返回值
local function minMax(a, b)
    if a < b then return a, b else return b, a end
end
local min, max = minMax(10, 3)
print(min, max)             -- 3  10

-- 可变参数
local function sum(...)
    local total = 0
    for _, v in ipairs({...}) do
        total = total + v
    end
    return total
end
print(sum(1, 2, 3, 4))      -- 10

-- ============ 闭包 ============
local function counter()
    local n = 0
    return function()
        n = n + 1
        return n
    end
end
local c = counter()
print(c(), c(), c())        -- 1  2  3

-- ============ 面向对象（用表模拟）============
local Animal = {}
Animal.__index = Animal

function Animal.new(name)
    local self = setmetatable({}, Animal)
    self.name = name
    return self
end

function Animal:speak()
    print(self.name .. " 发出声音")
end

local Dog = setmetatable({}, { __index = Animal })
Dog.__index = Dog

function Dog.new(name)
    local self = Animal.new(name)
    return setmetatable(self, Dog)
end

function Dog:speak()
    print(self.name .. " 汪汪叫")
end

local d = Dog.new("旺财")
d:speak()                   -- 旺财 汪汪叫

-- ============ 错误处理 ============
local ok, err = pcall(function()
    error("出错了！")
end)
print(ok, err)              -- false  ...出错了！

-- ============ 标准库示例 ============
print(math.floor(3.7))      -- 3
print(string.upper("lua"))  -- LUA
print(table.concat(fruits, ", "))  -- apple, banana, orange