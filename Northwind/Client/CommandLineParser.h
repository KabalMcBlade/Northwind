#pragma once

#include "../Dependencies/Eos/Eos/Eos.h"

#include "../Core/MemoryWrapper.h"
#include "../Core/BasicDefines.h"
#include "../Core/BasicTypes.h"


NW_NAMESPACE_BEGIN

class NW_DLL Option
{
public:
	NW_INLINE Option() : m_isMandatory(false), m_isSet(false) {}
	NW_INLINE virtual ~Option() {}

	NW_INLINE virtual bool HasValue() const { return false; }
	NW_INLINE virtual bool NeedValue() const { return false; }
	NW_INLINE virtual bool HasDefault() const { return false; }

	NW_INLINE void SetOption(const nwString& _option) { m_option = _option; }
	NW_INLINE bool IsSet() const { return m_isSet; }
	NW_INLINE bool IsMandatory() const { return m_isMandatory; }
	NW_INLINE void Set() { m_isSet = true; }
	NW_INLINE void SetMandatory(bool _isMandatory) { m_isMandatory = _isMandatory; }
	NW_INLINE const nwString& GetOption() const { return m_option; }
	NW_INLINE void SetStringValue(const nwString& _value)
	{
		m_stringValue = _value;
		std::transform(m_stringValue.begin(), m_stringValue.end(), m_stringValue.begin(), ::tolower);
	}

protected:
	friend class CommandLineParser;
	using OptionAllocator = eos::MemoryAllocator<eos::FreeListBestSearchAllocationPolicy, eos::MultiThreadPolicy, eos::MemoryBoundsCheck, eos::MemoryTag, eos::MemoryLog>;
	static OptionAllocator* GetAllocator();

	nwString m_option;
	nwString m_stringValue;
	bool m_isMandatory;
	bool m_isSet;
};

//////////////////////////////////////////////////////////////////////////

template <class T>
class OptionValue : public Option
{
public:
	OptionValue() : Option(), m_hasDefault(false) {}
	virtual ~OptionValue() {}
	virtual bool NeedValue() const override { return true; }
	virtual bool HasValue() const override { return !m_stringValue.empty(); }
	virtual bool HasDefault() const override { return m_hasDefault; }
	void SetDefault(T _value)
	{
		m_default = _value;
		m_hasDefault = true;
	}
	const T& GetValue() const { return HasValue() ? m_value : m_default; }
	void Parse()
	{
		T var;
		eos::IStringStream<Option::OptionAllocator, Option::GetAllocator> iss;
		iss.str(m_stringValue.c_str());
		iss >> var;
		m_value = var;
	}

private:
	T m_value;
	T m_default;
	bool m_hasDefault;
};

//////////////////////////////////////////////////////////////////////////

template <>
class OptionValue<nwString> : public Option
{
public:
	NW_INLINE OptionValue() : Option(), m_hasDefault(false) {}
	NW_INLINE virtual ~OptionValue() {}

	NW_INLINE virtual bool NeedValue() const override { return true; }
	NW_INLINE virtual bool HasValue() const override { return !m_stringValue.empty(); }
	NW_INLINE virtual bool HasDefault() const override { return m_hasDefault; }
	NW_INLINE void SetDefault(nwString _value)
	{
		m_default = _value;
		m_hasDefault = true;
	}

	NW_INLINE const nwString& GetValue() const { return HasValue() ? m_value : m_default; }
	NW_INLINE void Parse() { m_value = m_stringValue; }

private:
	nwString m_value;
	nwString m_default;
	bool m_hasDefault;
};

//////////////////////////////////////////////////////////////////////////

class NW_DLL CommandLineParser final
{
public:
	CommandLineParser();
	~CommandLineParser();

	void Add(const nwString& _option, bool _mandatory = true);

	template <class T>
	void AddWithValue(const nwString& _option, bool _mandatory = true)
	{
		OptionValue<T>* opt = ionNew(OptionValue<T>);
		opt->SetOption(_option);
		opt->SetMandatory(_mandatory);

		m_options[_option] = opt;
	}

	template <class T>
	void AddWithValueAndDefault(const nwString& _option, bool _mandatory = true, const T _default = T())
	{
		OptionValue<T>* opt = eosNew(OptionValue<T>, Option::GetAllocator());
		opt->SetOption(_option);
		opt->SetMandatory(_mandatory);
		opt->SetDefault(_default);
		opt->SetStringValue(std::to_string(_default).c_str());  // redundant but easy

		m_options[_option] = opt;
	}


	bool Parse(int32 argc, const char * const argv[]);

	bool HasOption(const nwString& _option);

	bool HasValue(const nwString& _option);
	bool IsSet(const nwString& _option);

	template <class T>
	const T GetValue(const nwString& _option, const T _default = T()) const
	{
		auto search = m_options.find(_option);
		if (search != m_options.end())
		{
			OptionValue<T>* opt = dynamic_cast<OptionValue<T>*>(search->second);
			if (opt->HasValue())
			{
				opt->Parse();
				return opt->GetValue();
			}
			else
			{
				return _default;
			}
		}
		else
		{
			return _default;
		}
	}

private:
	eos::Map<nwString, Option*, Option::OptionAllocator, Option::GetAllocator> m_options;
};


NW_NAMESPACE_END