#### NOTICE: THIS FILE IS AUTOGENERATED
#### MODIFICATIONS MAY BE LOST IF DONE IMPROPERLY
#### PLEASE SEE THE ONLINE DOCUMENTATION FOR EXAMPLES

from swgpy.object import *

class Template(BaseTemplate):
	name = "object/static/item/shared_item_container_plain_s01.iff"
	is_prototype = False
	
	def create(self, kernel, params):
		result = Static()
	
		result.template = "object/static/item/shared_item_container_plain_s01.iff"
		result.attribute_template_id = -1
		result.stfName("obj_n","unknown_object")		
		
		#### BEGIN MODIFICATIONS ####
		####  END MODIFICATIONS  ####
		
		return result

def loadTemplates(addTemplate):
	addTemplate(Template())