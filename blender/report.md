# Professor expectations:

**Modelling techniques part I**
-	Basic editing 
-	Box-modeling
-	Proportional editing
-	Parenting and tracking
-	DupliVerts
-	Boolean operations
-	Blueprints technique
-	
**Modelling techniques part II**
-	Curves and surfaces
-	Skinning technique
-	Spin editing
-	Lattices
-	Multiresolution meshes
-	 Sculpting
-	 
**Texturing**
-	materials
-	Procedural and Image Textures
-	UV Mapping
-	Normal Mapping and Bump Mapping
-	Displacement Mapping

**Physics:**
-	Particle Systems :  Emitter, Hair
-	Soft Bodies
-	Cloth effect
-	Fluid Simulation : Liquid simulation ; Gas/Smoke simulation
-	Dynamic Painting
-	Metaball
-	Rigid Body


# Blender Project – explanatory note

The aim of this note is to list the techniques that have been used in our Blender Project. We will focus on the notable techniques that have been used to model and texture objects, as well as to simulate real-world interactions and physics. The basic modelling techniques – extrude, rotate, scale, bevel, loop cut, shade smooth, and toggle X-ray – were used in the modelling of almost all the objects and will not be described here. Note that we used the “BlenderKit Online Asset Library” add-on for some textures. 


## PLANTS - POTS :
-	**Modelling**:
  - spin editing for the pots
  -	proportional editing to curve the leaf
  -	solidify modifier on the leaf
  -	array modifier on the leaf. Select ‘object offset’ and make it point at an empty sphere. Set the leaf count to 25. Then, move and rotate the empty sphere along the Z-axis to distribute the leaves along the stem. Scaling the sphere proportionally decreases the size of the leaves from top to bottom.
  -	Curve modifier on the stem and on the leaf. The modifier points to a Bezier curve and the deformation axis is set to the Z-axis. 
  -	Proportional editing is used to modify the shape of the leaf to create different types of plants. 
-	**Texturing**:
  -	The UV editor is used to unwrap the different types of leaves and to map them with an image (.png). 
  -	We supplement the texture of the leaves by using transparent and translucent BSDF shaders that are linked with a mix shader.
  -	We apply a displace modifier to the mud object to create relief in the pots
-	**Tutorials** : 
  -	yojigraphics , « Blender 2 8 création d’une plante modulable avec les modifiers », 28/05/2019. Url : https://www.youtube.com/watch?v=XV-v70wPX3w. 
  -	ZhaoyangGao, « Making leaves translucent in blender”, 02/04/2024. Url: https://www.youtube.com/watch?v=AhFe0hgmHio. 

