#### NOTICE: THIS FILE IS AUTOGENERATED
#### MODIFICATIONS MAY BE LOST IF DONE IMPROPERLY
#### PLEASE SEE THE ONLINE DOCUMENTATION FOR EXAMPLES

from swgpy.object import *

class Template(BaseTemplate):
	name = "object/ship/player/shared_player_tie_light_duty.iff"
	is_prototype = False
	
	def create(self, kernel, params):
		result = Ship()
	
		result.template = "object/ship/player/shared_player_tie_light_duty.iff"
		result.attribute_template_id = -1
		result.stfName("space_ship","player_tie_light_duty")		
		
		#### BEGIN MODIFICATIONS ####
		####  END MODIFICATIONS  ####
		
		return result

def loadTemplates(addTemplate):
	addTemplate(Template())