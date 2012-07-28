function love.load()
	image = love.graphics.newImage(love.image.newImageData(1, 1))
	
	mapsize = { w = 15, h = 15, d = 15 }
	mapx = 380
	mapy = 280
	
	tilewidth = 20
	tileheight = 10
	tiledepth = 10
	
	love.graphics.setCaption("Very usable, convinient and polished level editor")
	
	tileImg = love.graphics.newImage("isotile.png")
	
	selection = {x = 7, y = 5, z = 2}
	currcolor = {r = 255, g = 0, b = 0}
	
	map = {}
	for x = 1, mapsize.w do
		map[x] = {}
		for y = 1, mapsize.h do
			map[x][y] = {}
			for z = 1, mapsize.d do
				if z == 1 then
					map[x][y][z] = { empty = false, r = 130, g = 230, b = 80 }
				else
					map[x][y][z] = { empty = true, r = 255, g = 0, b = 255 }
				end
			end
		end
	end
	
	hueSnip = "vec3 hsv(float h,float s,float v) { return mix(vec3(1.),clamp((abs(fract(h+vec3(3.,2.,1.)/3.)*6.-3.)-1.),0.,1.),s)*v; }"
	
	hueBar = love.graphics.newPixelEffect(hueSnip .. [[
		vec4 effect(vec4 color, Image texture, vec2 texture_coords, vec2 screen_coords)
		{
			return(vec4(hsv(texture_coords.y, 1.0, 1.0), 1.0));
		}
	]])
	satBar = love.graphics.newPixelEffect(hueSnip .. [[
		extern float hue;
		vec4 effect(vec4 color, Image texture, vec2 texture_coords, vec2 screen_coords)
		{
			return(vec4(hsv(hue, 1.-texture_coords.y, 1.0), 1.0));
		}
	]])
	valBar = love.graphics.newPixelEffect(hueSnip .. [[
		extern float hue;
		extern float sat;
		vec4 effect(vec4 color, Image texture, vec2 texture_coords, vec2 screen_coords)
		{
			return(vec4(hsv(hue, sat, 1.-texture_coords.y), 1.0));
		}
	]])
	hue, sat, val = 0, 1, 1
	valBar:send("sat", sat)
end

function love.update(dt)
	msx, msy = love.mouse.getPosition()
	if msx >= 20 and msx <= 35 and msy >= 20 and msy <= 140 and love.mouse.isDown("l") then
		hue = ((msy-20)) / 120
		satBar:send("hue", hue)
		valBar:send("hue", hue)
	end
	if msx >= 45 and msx <= 60 and msy >= 20 and msy <= 140 and love.mouse.isDown("l") then
		sat = 1-((msy-20)) / 120
		valBar:send("sat", sat)
	end
	if msx >= 70 and msx <= 85 and msy >= 20 and msy <= 140 and love.mouse.isDown("l") then
		val = 1-((msy-20)) / 120
	end
end

function love.draw()
	love.graphics.setColor(255, 255, 255)
	love.graphics.setPixelEffect(hueBar)
	love.graphics.draw(image, 20, 20, 0, 15, 120)
	love.graphics.setPixelEffect(satBar)
	love.graphics.draw(image, 45, 20, 0, 15, 120)
	love.graphics.setPixelEffect(valBar)
	love.graphics.draw(image, 70, 20, 0, 15, 120)
	love.graphics.setPixelEffect()
	
	love.graphics.line(18, hue*120+20, 38, hue*120+20)
	love.graphics.line(43, 120-sat*120+20, 63, 120-sat*120+20)
	love.graphics.line(68, 120-val*120+20, 88, 120-val*120+20)
	
	love.graphics.print("Current color: ", 95, 20)
	local r, g, b = HSV(hue*255, sat*255, val*255)
	love.graphics.setColor(r, g, b)
	currcolor.r = r
	currcolor.g = g
	currcolor.b = b
	love.graphics.rectangle("fill", 95, 40, 84, 15)
	
	
	for x = 1, mapsize.w do
		for y = 1, mapsize.h do
			for z = 1, mapsize.d do
				if not map[x][y][z].empty then
					love.graphics.setColor(map[x][y][z].r, map[x][y][z].g, map[x][y][z].b)
					local tx, ty = (tilewidth/2)*(y-x), (tileheight/2)*(y+x)-z*tiledepth
					--love.graphics.rectangle("fill", tx+mapx, ty+mapy, 2, 2)
					love.graphics.draw(tileImg, tx+mapx, ty+mapy)
				end
			end
		end
	end
	
	local sx, sy = (tilewidth/2)*(selection.y-selection.x), (tileheight/2)*(selection.y+selection.x)-selection.z*tiledepth
	love.graphics.setColor(255, 0, 0, 100)
	love.graphics.draw(tileImg, sx+mapx, sy+mapy)
end

function love.keypressed(key)
	if key == "escape" then love.event.push("quit") end
	
	keyUp = love.keyboard.isDown("w")
	keyDown = love.keyboard.isDown("s")
	keyLeft = love.keyboard.isDown("a")
	keyRight = love.keyboard.isDown("d")
	keyTop = love.keyboard.isDown("up")
	keyBottom = love.keyboard.isDown("down")
	keyAction = love.keyboard.isDown(" ")
	keyClear = love.keyboard.isDown("return")
	
	if keyUp then
		selection.x = selection.x-1
	elseif keyDown then
		selection.x = selection.x+1
	elseif keyLeft then
		selection.y = selection.y-1
	elseif keyRight then
		selection.y = selection.y+1
	elseif keyBottom then
		selection.z = selection.z-1
	elseif keyTop then
		selection.z = selection.z+1
	end
	
	if keyAction then
		if selection.x >= 1 and selection.x <= mapsize.w and selection.y >= 1 and selection.y <= mapsize.h and selection.z >= 1 and selection.z <= mapsize.d then
			map[selection.x][selection.y][selection.z].empty = false
			map[selection.x][selection.y][selection.z].r = currcolor.r
			map[selection.x][selection.y][selection.z].g = currcolor.g
			map[selection.x][selection.y][selection.z].b = currcolor.b
		end
	end
	if keyClear then
		if selection.x >= 1 and selection.x <= mapsize.w and selection.y >= 1 and selection.y <= mapsize.h and selection.z >= 1 and selection.z <= mapsize.d then
			map[selection.x][selection.y][selection.z].empty = not map[selection.x][selection.y][selection.z].empty
		end
	end
end

function HSV(h, s, v)
    if s <= 0 then return v,v,v end
    h, s, v = h/256*6, s/255, v/255
    local c = v*s
    local x = (1-math.abs((h%2)-1))*c
    local m,r,g,b = (v-c), 0,0,0
    if h < 1     then r,g,b = c,x,0
    elseif h < 2 then r,g,b = x,c,0
    elseif h < 3 then r,g,b = 0,c,x
    elseif h < 4 then r,g,b = 0,x,c
    elseif h < 5 then r,g,b = x,0,c
    else              r,g,b = c,0,x
    end return (r+m)*255,(g+m)*255,(b+m)*255
end