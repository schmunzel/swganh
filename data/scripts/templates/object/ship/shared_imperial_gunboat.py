#### NOTICE: THIS FILE IS AUTOGENERATED
#### MODIFICATIONS MAY BE LOST IF DONE IMPROPERLY
#### PLEASE SEE THE ONLINE DOCUMENTATION FOR EXAMPLES

from swgpy.object import *

class Template(BaseTemplate):
	name = "object/ship/shared_imperial_gunboat.iff"
	is_prototype = False
	
	def create(self, kernel, params):
		result = Ship()
	
		result.template = "object/ship/shared_imperial_gunboat.iff"
		result.attribute_template_id = -1
		result.stfName("space_ship","imperial_gunboat")		
		
		#### BEGIN MODIFICATIONS ####
		####  END MODIFICATIONS  ####
		
		return result

def loadTemplates(addTemplate):
	addTemplate(Template())