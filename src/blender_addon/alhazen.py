import bpy
from bpy.props import FloatProperty
import bgl
import gpu

bl_info = {
    "name": "Scale222",
    "author": "Nutti",
    "version": (2, 0),
    "blender": (2, 79, 0),
    "location": "",
    "description": "THIS IS 0",
    "warning": "",
    "support": "TESTING",
    "wiki_url": "",
    "tracker_url": "",
    "category": "Object"
}

def main():
    imgName = "ALHAZEN"
    imgSize = [256,256]
    images = bpy.data.images
    #
    if images.find(imgName) == -1:
        images.new(name=imgName,width=imgSize[0],height=imgSize[1]) 
    img = images[imgName] 
    print(dir(img))
    pixels = [None] * imgSize[0] * imgSize[1]
    for x in range(imgSize[0]):
        for y in range(imgSize[1]):
            r = x / imgSize[0]
            g = y / imgSize[1]
            b = (1 - r) * g
            a = 1.0
            pixels[(y * imgSize[0]) + x] = [r, g, b, a]
            pixels[(y * imgSize[0]) + x] = [0xFF, g, b, a]

    pixels = [chan for px in pixels for chan in px]
    img.pixels = pixels

#
class CreateObject(bpy.types.Operator):
    bl_idname = "object.create_object"
    bl_label = "MyTest3"
    bl_description = "Add Cube!!!"
    bl_options = {'REGISTER', 'UNDO'}
    magnification = FloatProperty(
        name="scale!",
        description="set scale",
        default=1.0,
        min=0.1,
        max=10.0
    )

    #
    def execute(self, context):
        main();
        print("DOOOOOOOOOOOn")
        return {'FINISHED'}


#
def menu_fn(self, context):
    self.layout.separator()
    self.layout.operator(CreateObject.bl_idname)


def register():
    print("Register")
    bpy.utils.register_module(__name__)
    bpy.types.INFO_MT_mesh_add.append(menu_fn)


def unregister():
    bpy.types.INFO_MT_mesh_add.remove(menu_fn)
    bpy.utils.unregister_module(__name__)


if __name__ == "__main__":
    register()
