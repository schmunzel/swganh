#### NOTICE: THIS FILE IS AUTOGENERATED
#### MODIFICATIONS MAY BE LOST IF DONE IMPROPERLY
#### PLEASE SEE THE ONLINE DOCUMENTATION FOR EXAMPLES

from swgpy.object import *

class Template(BaseTemplate):
	name = "object/factory/shared_factory_crate_chemicals.iff"
	is_prototype = False
	
	def create(self, kernel, params):
		result = FactoryCrate()
	
		result.template = "object/factory/shared_factory_crate_chemicals.iff"
		result.attribute_template_id = 4
		result.stfName("factory_n","chemicals_crate")		
		
		#### BEGIN MODIFICATIONS ####
		####  END MODIFICATIONS  ####
		
		return result

def loadTemplates(addTemplate):
	addTemplate(Template())