#### NOTICE: THIS FILE IS AUTOGENERATED
#### MODIFICATIONS MAY BE LOST IF DONE IMPROPERLY
#### PLEASE SEE THE ONLINE DOCUMENTATION FOR EXAMPLES

from swgpy.object import *

class Template(BaseTemplate):
	name = "object/ship/shared_escape_pod.iff"
	is_prototype = False
	
	def create(self, kernel, params):
		result = Ship()
	
		result.template = "object/ship/shared_escape_pod.iff"
		result.attribute_template_id = -1
		result.stfName("","xwing swg-sw test")		
		
		#### BEGIN MODIFICATIONS ####
		####  END MODIFICATIONS  ####
		
		return result

def loadTemplates(addTemplate):
	addTemplate(Template())