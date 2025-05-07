local zstd = require "zstd"


print("zstd version: ", zstd.version_number(), zstd.version())

function getNstr(n)
    local t = {}
    for i = 1, n do
        local c = math.random(65, 122)
        t[i] = string.char(c)
    end
    return table.concat(t)
end

function test_one()
    local str = getNstr(5000)

    local s = zstd.compress(str)
    print(#str, #s)
    assert(zstd.decompress(s) == str)

    local s = zstd.compress(str, 1)
    print(#str, #s)
    assert(zstd.decompress(s) == str)
end
test_one()

function testMem()
    n = 10000000
    for i = 1, n do
        local str = getNstr(1000)
        local s = zstd.compress(str)
        -- print(s, string.len(s))
        assert(zstd.decompress(s) == str)
        print(i, #str, #s)
    end
end
-- testMem()