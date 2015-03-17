classes = {}
setmetatable( classes, { __mode = 'k' } )

function create( name )

    local class = {}
    setmetatable( class,
    {
        __gc = function( self )
            print( self.name .. ' is being finalized.' )
            if self.inner then
                print( 'self.inner is not nil' )
                print( 'It\'s name was ' .. self.inner.name )
            end
            if classes[self.inner] ~= nil then
                print( 'Inner is still in the classes table.' )
            end
            classes[self] = nil
        end
    } )

    class.name = name

    classes[class] = true

    return class
end

inner = create 'Inner'
outer = create 'Outer'

outer.inner = inner
inner = nil

-- Some time later....

outer = nil

collectgarbage();

print 'Fin'
