--[[ Copyright (C) 2013-2018 PUC-Rio/Laboratorio TeleMidia

This file is part of NCLua.

NCLua is free software: you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

NCLua is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
License for more details.

You should have received a copy of the GNU General Public License
along with NCLua.  If not, see <https://www.gnu.org/licenses/>.  ]]--


local ZIPNAME = "test-event-zip.zip"
local TESTDATA1 = "Some test data 1...\0"
local TESTDATA2 = "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ullamcorper sit amet risus nullam eget. Eget mi proin sed libero. Tristique senectus et netus et malesuada fames. Amet mauris commodo quis imperdiet massa tincidunt nunc pulvinar. Vitae elementum curabitur vitae nunc sed velit dignissim. A cras semper auctor neque vitae tempus quam. Velit sed ullamcorper morbi tincidunt ornare massa. Tempor commodo ullamcorper a lacus. Et tortor consequat id porta. Purus viverra accumsan in nisl nisi scelerisque eu. Nunc lobortis mattis aliquam faucibus purus. Id donec ultrices tincidunt arcu non sodales neque sodales.Orci nulla pellentesque dignissim enim sit amet venenatis.Tellus mauris a diam maecenas sed enim ut sem viverra.Eleifend donec pretium vulputate sapien nec sagittis aliquam malesuada.Aenean et tortor at risus viverra adipiscing.Mauris cursus mattis molestie a iaculis at erat.Arcu non sodales neque sodales ut etiam.Semper auctor neque vitae tempus quam pellentesque.Sed turpis tincidunt id aliquet risus.Nunc mi ipsum faucibus vitae.Turpis cursus in hac habitasse platea.Arcu vitae elementum curabitur vitae.Condimentum mattis pellentesque id nibh.Turpis cursus in hac habitasse platea dictumst.Viverra justo nec ultrices dui sapien eget mi proin sed.gnissim convallis aenean et tortor at risus viverra adipiscing at.Mi tempus imperdiet nulla malesuada pellentesque elit eget gravida.Mattis pellentesque id nibh tortor id.Ut venenatis tellus in metus.Volutpat maecenas volutpat blandit aliquam etiam erat velit scelerisque.Ornare arcu dui vivamus arcu felis.Nulla facilisi cras fermentum odio eu feugiat pretium nibh ipsum.Sit amet mauris commodo quis imperdiet massa tincidunt nunc.Vulputate ut pharetra sit amet aliquam id.At augue eget arcu dictum varius duis at consectetur.Aliquam sem fringilla ut morbi tincidunt augue interdum.Aliquam vestibulum morbi blandit cursus risus at ultrices mi tempus. Faucibus turpis in eu mi bibendum neque.Velit egestas dui id ornare.Odio eu feugiat pretium nibh.Facilisis leo vel fringilla est ullamcorper eget nulla facilisi.Egestas egestas fringilla phasellus faucibus scelerisque eleifend donec pretium vulputate.Netus et malesuada fames ac turpis.Sapien pellentesque habitant morbi tristique.Magna sit amet purus gravida quis blandit turpis cursus in.Morbi tempus iaculis urna id volutpat lacus laoreet non curabitur.Morbi tincidunt augue interdum velit euismod in pellentesque.Consectetur lorem donec massa sapien faucibus et molestie.Fermentum et sollicitudin ac orci phasellus.Vel pretium lectus quam id leo in.Nisl nisi scelerisque eu ultrices.Viverra mauris in aliquam sem.Scelerisque eleifend donec pretium vulputate sapien nec sagittis aliquam malesuada.Feugiat nibh sed pulvinar proin gravida hendrerit lectus a.Non odio euismod lacinia at quis.Nulla pellentesque dignissim enim sit amet venenatis urna cursus eget. Tempor nec feugiat nisl pretium fusce id velit ut.Ornare suspendisse sed nisi lacus sed viverra tellus in.Odio tempor orci dapibus ultrices in iaculis nunc sed augue.Leo urna molestie at elementum eu facilisis.Quis vel eros donec ac.In hac habitasse platea dictumst quisque sagittis.Facilisi nullam vehicula ipsum a arcu cursus vitae.Sapien eget mi proin sed libero enim sed faucibus turpis.Eget lorem dolor sed viverra ipsum nunc aliquet.Amet massa vitae tortor condimentum.Arcu dui vivamus arcu felis bibendum ut tristique et egestas.Justo laoreet sit amet cursus sit amet dictum sit amet. Commodo quis imperdiet massa tincidunt nunc pulvinar sapien et.Quisque sagittis purus sit amet.Augue mauris augue neque gravida in fermentum et.Nunc pulvinar sapien et ligula ullamcorper malesuada proin libero.Congue mauris rhoncus aenean vel elit scelerisque mauris.Quam quisque id diam vel quam.Nunc congue nisi vitae suscipit.Tristique sollicitudin nibh sit amet commodo nulla.Nulla facilisi morbi tempus iaculis.Turpis massa tincidunt dui ut ornare.Neque aliquam vestibulum morbi blandit cursus.Ullamcorper a lacus vestibulum sed arcu non odio euismod.Sem viverra aliquet eget sit amet tellus cras adipiscing.Dui vivamus arcu felis bibendum ut tristique.Ipsum faucibus vitae aliquet nec ullamcorper sit amet risus nullam.Vel pharetra vel turpis nunc eget lorem dolor sed.Purus gravida quis blandit turpis cursus. Varius morbi enim nunc faucibus a.Orci nulla pellentesque dignissim enim sit amet.Libero nunc consequat interdum varius.Adipiscing elit ut aliquam purus.Neque sodales ut etiam sit amet nisl purus.Quis commodo odio aenean sed.Eu mi bibendum neque egestas congue quisque egestas diam.Eget felis eget nunc lobortis.Tristique sollicitudin nibh sit amet commodo nulla facilisi nullam vehicula.Eget nulla facilisi etiam dignissim diam quis.Consectetur adipiscing elit duis tristique sollicitudin nibh sit amet commodo.Lorem ipsum dolor sit amet consectetur adipiscing elit.Euismod nisi porta lorem mollis aliquam ut porttitor leo.Ullamcorper morbi tincidunt ornare massa eget egestas purus viverra accumsan.Sed risus ultricies tristique nulla aliquet enim.Lacus viverra vitae congue eu consequat.Ac auctor augue mauris augue neque gravida in fermentum.Lacus luctus accumsan tortor posuere ac ut.Quis eleifend quam adipiscing vitae. Purus sit amet volutpat consequat mauris nunc.Vestibulum mattis ullamcorper velit sed.Quam adipiscing vitae proin sagittis nisl rhoncus mattis.Tristique sollicitudin nibh sit amet.Placerat in egestas erat imperdiet sed.Molestie at elementum eu facilisis sed odio morbi quis.Sit amet venenatis urna cursus eget nunc scelerisque viverra mauris.Ut etiam sit amet nisl purus in mollis nunc.Iaculis nunc sed augue lacus viverra.Diam phasellus vestibulum lorem sed risus ultricies tristique nulla aliquet.Tellus cras adipiscing enim eu turpis egestas pretium aenean pharetra.In massa tempor nec feugiat. Scelerisque fermentum dui faucibus in ornare quam viverra orci.Massa id neque aliquam vestibulum.Pharetra pharetra massa massa ultricies mi quis hendrerit.Iaculis eu non diam phasellus vestibulum lorem.Leo a diam sollicitudin tempor id eu nisl nunc mi.Aliquam ut porttitor leo a diam sollicitudin tempor id.Lectus sit amet est placerat in egestas erat.Placerat duis ultricies lacus sed.Quis viverra nibh cras pulvinar mattis nunc.Mollis aliquam ut porttitor leo a diam sollicitudin tempor id.Mauris augue neque gravida in fermentum et.Dictum non consectetur a erat nam at.Tortor pretium viverra suspendisse potenti nullam ac tortor.Sed lectus vestibulum mattis ullamcorper velit sed ullamcorper. Ac tortor dignissim convallis aenean et tortor at.Cras semper auctor neque vitae tempus quam pellentesque.Tristique senectus et netus et malesuada fames.A pellentesque sit amet porttitor eget dolor morbi non arcu.Eleifend mi in nulla posuere sollicitudin.Nunc lobortis mattis aliquam faucibus.Adipiscing enim eu turpis egestas.Bibendum est ultricies integer quis auctor elit sed vulputate.In arcu cursus euismod quis viverra nibh cras pulvinar mattis.Proin nibh nisl condimentum id venenatis a condimentum vitae sapien.Phasellus egestas tellus rutrum tellus.Non arcu risus quis varius quam quisque id.Ac feugiat sed lectus vestibulum mattis.Nisl nisi scelerisque eu ultrices.Nulla at volutpat diam ut venenatis tellus.Curabitur gravida arcu ac tortor dignissim convallis aenean."


local tests = require ('tests')
local ASSERT = tests.ASSERT

local zip = require ('nclua.event.zip')


local function AWAIT_EVENT ()
   tests.dump ('cycling')
   local evt = nil
   repeat
      zip:cycle ()
      tests.dump("esperando")
      evt = zip:receive ()
   until evt ~= nil
   return evt
end






-- Check class-only.
local t = zip:filter ('zip')
ASSERT (tests.objeq (t, {class='zip'}))

-- Check class and type.
local t1 = zip:filter ('zip', 'open')
ASSERT (tests.objeq (t1, {class='zip', type='open'}))

local t2 = zip:filter ('zip', 'extract')
ASSERT (tests.objeq (t2, {class='zip', type='extract'}))

local t3 = zip:filter ('zip', 'save')
ASSERT (tests.objeq (t3, {class='zip', type='save'}))

local t4 = zip:filter ('zip', 'cancel')
ASSERT (tests.objeq (t4, {class='zip', type='cancel'}))

---[[
pilha = zip:send{
	class 	= 'zip',
	type 	= 'open',
   mode  = 'w',
	path 	= ZIPNAME,
}


tests.dump(pilha)

evt = AWAIT_EVENT()

tests.dump(evt)

local teste = evt.zip


local nome  = "do_lua.txt"
local texto = "0123456789"
---[[
zip:addFile(teste, nome, texto)

--]]

zip:close(teste)