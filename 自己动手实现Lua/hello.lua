function foo()
    print(123)
end

function fib(n)
    if n < 2 then
        return n
    end
    return fib(n-2)+fib(n-1)
end

local start = os.clock()
fib(35)
print(os.clock() - start)