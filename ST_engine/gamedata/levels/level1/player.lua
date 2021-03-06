-- This file is part of the "ST" project.
-- You may use, distribute or modify this code under the terms
-- of the GNU General Public License version 2.
-- See LICENCE.txt in the root directory of the project.
--
-- Author: Maxim Atanasov
-- E-mail: maxim.atanasov@protonmail.com

player = entity:new()
player.texture = "player2.webp"
player.texWidth = 300
player.texHeight = 305
player.offsetColX = 50
player.offsetColY = 0
player.colX = 65
player.colY = 305
player.animationNum = 6
player.spriteNum = 6
player.idleRightAnim = 1
player.idleLeftAnim = 2
player.leftAnim = 3
player.rightAnim = 4
player.jumpLeft = 5
player.jumpRight = 6
player.lastDirection = 0
player.jumpC = 0
player.isVisible = true
player.prevY = 0
player.affectedByPhysics = true
player.mass = 10;

function player:update()
    leftStick = leftStickHorizontal()
    if leftStick < -2000 and leftStick > -15000 then
       self:moveLeft(5)
    elseif leftStick < -15000 then
        self:moveLeft(10)
    elseif leftStick > 2000 and leftStick < 15000 then
        self:moveRight(5)
    elseif leftStick > 15000 then
        self:moveRight(10)
    elseif keyHeld("LEFT") then
        self:moveLeft(10)
    elseif keyHeld("RIGHT") then
        self:moveRight(10)
    else
        self:idle()
    end
    if keyPressed("JUMP") then
        self:jump()
    end

    --handle jumping
    if self:getVelocityY() < 0 then
        if self.lastDirection == 1 then
            self:playAnimation(self.jumpLeft)
        elseif self.lastDirection == 0 then
            self:playAnimation(self.jumpRight)
        end
    end
    self.prevY = self:getY()

    if self:overObject(wolfTrigger1) then
        self:setCollision(0, 0, 0, 0)
        controllerRumble(0.5, 1000)
        startLevel("level1")
    end

    if self:overObject(grass26) then
        unloadLevel("level1")
        startLevel("levelCard")
    end
end

function player:jump()
    if self.prevY == self:getY() then
        self:setVelocityY(-47)
    end
end

function player:moveRight(speed)
    self.lastDirection = 0;
    self:setVelocityX(speed)
    self:playAnimation(self.rightAnim)
end

function player:moveLeft(speed)
    self:setVelocityX(-speed)
    self.lastDirection = 1;
    self:playAnimation(self.leftAnim)
end

function player:idle()
    if self.lastDirection == 0 then
        self:playAnimation(self.idleRightAnim)
    else
        self:playAnimation(self.idleLeftAnim)
    end
end
