// IOperation is base class

#pragma once
class IOperation
{
public:
	IOperation(void);
	virtual ~IOperation(void);
	virtual void undo()=0;
	virtual void redo()=0;
	virtual bool IsWaterOperation(){return false;}
	virtual bool IsGrassOperation(){return false;}
private:
	
};
