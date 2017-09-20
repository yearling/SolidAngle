# FbxSkeletonAnimation
主要由类FbxSkeleton来实现，FbxSkeleton继承于FbxNodeAttribute,类型为`FbxNodeAttribute::EType::eSkeleton`

## Bone type
	  enum EType
	  {
	    eRoot,			/*!< First element of a chain. */
	    eLimb,			/*!< Chain element.a bone defined by a transform and a length */
	    eLimbNode,		/*!< Chain element. a bone defined uniquely by a transform and a size value while*/
	    eEffector		/*!< Last element of a chain. */
	  };    

## LimbLength
>FbxSkeleton is a node attribute and it will be attached to a FbxNode which represents the transform.
Given a chain of skeleton nodes the parent and child skeletons will be attached to a parent node and a child node.
The orientation of the limb is computed from the vector between the parent and child position (from parent to child). 
The **LimbLength** represents the proportion 
of the parent node's position to the child node's position which is used to compute the actual limb length.
The default value of 1.0 means the LimbLength is equal to the length between the parent and child node's position.
So if the value is 0.5, it means the LimbLength will be half of the length between the parent and child node's position.
LimbLength是父子node节点实际距离的一个比值