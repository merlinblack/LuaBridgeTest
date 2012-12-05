debug.autostack=false

function i()
	local cmd, line, func, errmsg, quit, firstline, status, ret

	cmd = ''
	quit = false
	firstline = true

	while not quit do
		if debug.autostack == true then
			print( debug.traceback('',2) )
		end

		if firstline then
			io.write 'debug>'
		else
			io.write '>>'
		end
		line = io.read()
		if line == 'cont' then
			quit = true
		elseif line == 'stack' then
			print( debug.traceback('',2) )
		else
			if firstline == true and line:sub( 1, 1 ) == '=' then
				line = 'return ' .. line:sub( 2 )
			end

			cmd = cmd .. '\n' .. line

			func, errmsg = loadstring( cmd, '=stdin' )

			if func == nil then
				if errmsg:sub(-7) ~= "'<eof>'" then
					print(errmsg)
					cmd = ''
					firstline = true
				else
					firstline = false
				end
			else
				status, ret = pcall(func)
				if ret then
					print(ret)
				end
				cmd = ''
				firstline = true
			end
		end
	end
end
