#### NOTICE: THIS FILE IS AUTOGENERATED
#### MODIFICATIONS MAY BE LOST IF DONE IMPROPERLY
#### PLEASE SEE THE ONLINE DOCUMENTATION FOR EXAMPLES

from swgpy.object import *

class Template(BaseTemplate):
	name = "object/creature/npc/droid/crafted/shared_silver_protocol_droid.iff"
	is_prototype = False
	
	def create(self, kernel, params):
		result = Creature()
	
		result.template = "object/creature/npc/droid/crafted/shared_silver_protocol_droid.iff"
		result.attribute_template_id = 3
		result.stfName("droid_name","po_crafted")		
		
		#### BEGIN MODIFICATIONS ####
		####  END MODIFICATIONS  ####
		
		return result

def loadTemplates(addTemplate):
	addTemplate(Template())