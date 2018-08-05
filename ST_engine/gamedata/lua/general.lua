-- This file is part of the "slavicTales" project.
-- You may use, distribute or modify this code under the terms
-- of the GNU General Public License version 2.
-- See LICENCE.txt in the root directory of the project.
--
-- Author: Maxim Atanasov
-- E-mail: atanasovmaksim1@gmail.com


currentID = 0 --MUST start at 0
currentTextID = 0 --MUST start at 0
currentLightID = 0 --MUST start at 0

currentVolume = 100
math.randomseed(os.time())

--cleans up current level and start the specifed one
function startLevel(arg)
    startLevelLua(arg)
    currentID = 0
    currentLightID = 0
    currentTextID = 0
    error() --a dirty trick, but it works
end

function exit()
    endGame()
end
