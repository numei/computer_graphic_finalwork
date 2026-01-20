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

## FLOWERS : 
-	**Modelling**: 
    - An array modifier is applied to the petal. Select ‘object offset’ and make it point on an empty object. We set the petal count to 5. Then, to spread the petals, we rotate the empty object on the Z axis by 360/5 = 72°.
    - Proportional editing is then used to make the calyx fit the flower and to create different types of petals
-	**Texturing** : 
    - As we want the border of the petal to be more rippled than the centre, we create two vertex groups. A first displace modifier is used to create the global relief on the petal ; we choose ‘Clouds’ as the texture type and we increase the size of the texture to make it look smoother. The second displace modifier is applied to the vertex group containing the border vertices; the cloud texture is set to a small value to create the rippled effect we are looking for.
    - use of the translucent and transparent BSDF shaders for the petals
    - use of multiple materials in one mesh (eg. pistil and stamens are not coloured the same)
-	**Physics** : 
    - hair particle system is used to model the stamens on the pistil. We create a vertex group in the pistil object, which is then selected in the particle system parameters. In the ‘render’ category, we select the stamen object modelled for this purpose. To orient the stamens correctly, we set the orientation axis to normal. Finally, we make the instances real and join them.
-	**Tutorials**: 
    - Octopus Effects, “How to make Hibiscus flower in Blender – 219”, 10/08/2021. Url : https://www.youtube.com/watch?v=wRcL0CPljMA. 
    - Crazy Blender, “Wicker Basket Modeling Process In Blender (Real-Time)” 25/02/2024. Url: https://www.youtube.com/watch?v=BoegOqzqamE. 


## CHIMNEY – STONE FLOOR : 
-	**Modelling**
    - To create all the stones (on the wall, on the floor and on the work surface), we use the “cell fracture” quick effect (add-on) and the subdivision surface modifier (press CTRL[2] to apply the subdivision surface modifier to all selected components). The meshed stones are then joined with CTRL+J.
    - To create the large hole extrusion, a Boolean modifier with a sphere is used. 
-	**Texturing**: 
    - Tip: to link the material of the stones, CTRL+L >> link materials
    - For the stones and the wall, we load a texture with a bump map and a normal map from the Blender Toolkit add-on. To achieve the white and the beige colours, we add a colour node and a mix colour node plugged in the base colour property of the Principled BSDF
    - use of multiple materials in one mesh for the chimney metallic and black roof
-	**Tutorial**: Architecture Topics, “how to create a stone wall in Blender”, 19/08/2024. Url: https://www.youtube.com/watch?v=miUG801VlCA. 

## TABLE – WOODEN STOOL – CARPET - LAMPS
-	**Modelling** : 
    - Boolean and mirror modifiers on the wooden stools
    - Use of Bezier curves to model the two curved wooden laths of the table and the table stand. Then we did object >> convert >> mesh >> edit mode >> A >> F (fill) >> …
    - Solidify modifier on the lamps. Tip : to cut a straight cone through the volume, hold down CKEY+AKEY and cut.
-	**Texturing**: 
    - two emission shaders (one orange and one red) with a colour ramp factor are used for the light bulb 
    - an image texture is used for the carpet
    - For the lampshades, we load a texture from the blender Toolkit add-on with a normal map and a displacement node
-	**Tutorials**: Draizarg, “How to model a carpet in Blender?”,  23/02/2023. Url: https://www.youtube.com/watch?v=ezuCZau0eAg. 


## CAULDRON : 
-	**Modelling**: 
    - Mirror modifier on the caldron (handles and caldron legs)
    - Screw and curve modifier on the rope. The curve modifier points to a Bezier curve. 
    - Mirror modifier on the handle drawn with a Nurbs curve
-	**Texturing** : 
    - For the cauldron pot, a procedural texture (noise texture) and a bump map are used
    - for the handle, a procedural texture (noise texture) is used. To give the impression that the metal has been burnt, we used a colour mixer with a colour ramp. We set  the metallic property to 1.
    - For the Metaball material, we mix a transparent BSDF with a glass BSDF
    - For the water surface, we keep the default material and reduce its roughness almost to 0 and increase its transmission to 1
-	**Physics** : 
    - particles System (render as object, the instance object is a Metaball) and dynamic paint (brush type) on the bubbles
    - dynamic paint on the water surface (Canvas type)
    - collision on the cauldron 
    - quick smoke effect on a thin cylinder that has been placed just below the water level. 
-	**Tutorials** : 
    - Paulo Filipe Souza, “Blender Tutorial - Magic Cauldron with Liquid Animation Part 1”, 12/05/2020. Url : https://www.youtube.com/watch?v=x-6cvNjUuAI. 
    - Paulo Filipe Souza, “Blender Tutorial - Magic Cauldron with Liquid Animation Part 2”, 19/05/2020. Url : https://www.youtube.com/watch?v=RE3k01aC4Cc. 
    - Pixel Bender, “Boiling Water in Blender”, 05/05/2025. Url: https://www.youtube.com/watch?v=87DWXxBgzUs. 

## CANDLES : 
-	**Modelling** : we used a multiresolution modifier on the candle, which allowed us to use sculpt mode to model the melting wax
-	**Texturing** : use of two emission shaders with a colour ramp factor were used for the flame
-	**Tutorial**: Mykol, “BOUGIE AVEC SCULPT MODE ET FLAMME EN TEMPS REEL | Blender 2.8 [TUTO FR] », 08/08/2020. Url : https://www.youtube.com/watch?v=2ya9zHMXhq4. 

## WINDOW : 
-	**Modelling** : Boolean modifiers (use of a several cylinders)
-	**Texturing** : glass and wood textures

