-- This file is part of the "slavicTales" project.
-- You may use, distribute or modify this code under the terms
-- of the GNU General Public License version 2.
-- See LICENCE.txt in the root directory of the project.
--
-- Author: Maxim Atanasov
-- E-mail: atanasovmaksim1@gmail.com

playMusic("skowroneczek.ogg", 100, -1)
enableLighting(true)
stopAllSounds()

--Load the appropriate language configuration
if language == "russian" then
    use("language_russian.lua")
elseif language == "english" then
    use("language_english.lua")
end

label = label:new(960-LABEL_OFFSET, 870, LABEL_TEXT, FONT, FONT_SIZE)

setGravity(0)
setBackground("splash.webp")
setDarkness(254)
loadLevel("author_splash")
hideMouseCursor()
splash = 255
splashUp = 0

function loop()
    if splash > 0 then
        splash = splash - 1
        setDarkness(splash)
    elseif splashUp < 255 then
        splashUp = splashUp + 1
        setDarkness(splashUp)
    else
        unloadLevel("main")
        startLevel("author_splash")
    end

    if keyPressed("START") then
        unloadLevel("main")
        startLevel("author_splash")
    end
end